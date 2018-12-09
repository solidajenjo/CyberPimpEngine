#include <assert.h>
#include "Application.h"
#include "MaterialImporter.h"
#include "Assimp/include/assimp/material.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"
#include "DevIL/include/IL/ilu.h"
#include "ComponentMaterial.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include "rapidjson-1.1.0/include/rapidjson/error/en.h"

std::string MaterialImporter::Import(const aiMaterial* material, const unsigned index, GameObject* &materialImported) const
{
	assert(material != nullptr);
	xg::Guid guid = xg::newGuid();
	std::string uuid = guid.str();

	aiColor3D color(1.f, 1.f, 1.f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ComponentMaterial* materialComponent = new ComponentMaterial(color.r, color.g, color.b, 1.0f);
	std::string materialPath = "Library/Materials/" + uuid + "_" + std::to_string(index) + ".mat";
	
	std::string savePath = "";
	aiString file;
	aiReturn ret = material->GetTexture(aiTextureType_DIFFUSE, 0, &file, nullptr, nullptr);
	if (ret != AI_SUCCESS) 
	{
		LOG("Failed retrieving texture name.");
	}
	else
	{
		
		ILuint imageId;
		ilGenImages(1, &imageId);
		ilBindImage(imageId);
		if (ilLoadImage(file.C_Str())) //try in the same directory
		{
			savePath = "Library/Textures/" + std::string(file.C_Str()) + ".dds";
			ilSave(IL_DDS, savePath.c_str());
		}
		std::string alternativePath = "Assets/" + std::string(file.C_Str());
		if (ilLoadImage(alternativePath.c_str())) //try in Assets directory
		{
			savePath = "Library/Textures/" + std::string(file.C_Str()) + ".dds";
			ilSave(IL_DDS, savePath.c_str());
		}
		
	}
	
	sprintf_s(materialComponent->texturePath, savePath.c_str());
	sprintf_s(materialComponent->materialPath, materialPath.c_str());

	Save(materialPath.c_str(), materialComponent);
	
	materialImported->InsertComponent(materialComponent);
	
	ComponentMaterial::materialsLoaded[materialComponent->materialPath] = materialComponent;
	
	return materialPath;
}

ComponentMaterial* MaterialImporter::Load(const char path[4096]) const
{	
	unsigned size = App->fileSystem->Size(path);
	if (size > 0)
	{
		char* buffer = new char[size];
		if (App->fileSystem->Read(path, &buffer[0], size)) 
		{
			ComponentMaterial* newMat = new ComponentMaterial(1.f, 1.f, 1.f, 1.f);
			unsigned pointer = 0u;
			newMat->program = buffer[pointer];
			pointer += sizeof(unsigned);
			memcpy(&newMat->color.x, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);
			memcpy(&newMat->color.y, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);
			memcpy(&newMat->color.z, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);
			memcpy(&newMat->color.w, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);

			memcpy(&newMat->diffuse, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);
			memcpy(&newMat->ambient, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);
			memcpy(&newMat->specular, &buffer[pointer], sizeof(float));
			pointer += sizeof(float);
			memcpy(&newMat->texturePath, &buffer[pointer], sizeof(newMat->texturePath));
			if (strlen(newMat->texturePath) > 0)
			{
				newMat->texture = App->textures->Load(newMat->texturePath); //TODO: Control multi load
			}
			pointer += sizeof(newMat->texturePath);
			memcpy(&newMat->materialPath, &buffer[pointer], sizeof(newMat->materialPath));
			return newMat;

		}
	}
	return nullptr;
}

void MaterialImporter::Save(const char path[1024], const ComponentMaterial * material) const
{
	unsigned size = sizeof(unsigned) + sizeof(float) * 7 + sizeof(material->name) + sizeof(material->texturePath) + sizeof(material->materialPath);
	char* buffer = new char[size];
	unsigned pointer = 0u;
	memcpy(&buffer[pointer], &material->program, sizeof(unsigned));
	pointer += sizeof(unsigned);
	memcpy(&buffer[pointer], &material->color, sizeof(float) * 4);
	pointer += sizeof(float) * 4;
	memcpy(&buffer[pointer], &material->diffuse, sizeof(float));
	pointer += sizeof(float);
	memcpy(&buffer[pointer], &material->ambient, sizeof(float));
	pointer += sizeof(float);
	memcpy(&buffer[pointer], &material->specular, sizeof(float));
	pointer += sizeof(float);
	memcpy(&buffer[pointer], &material->texturePath, sizeof(material->texturePath));
	pointer += sizeof(material->texturePath);
	memcpy(&buffer[pointer], &material->materialPath, sizeof(material->materialPath));
	if (!App->fileSystem->Write(std::string(path), buffer, size))
	{
		LOG("Failed saving material %s", path);
	}
	RELEASE(buffer);
}

