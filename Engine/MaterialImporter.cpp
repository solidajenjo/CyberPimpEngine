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
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"

std::string MaterialImporter::Import(const aiMaterial* material, const unsigned index)
{
	assert(material != nullptr);
	xg::Guid guid = xg::newGuid();
	std::string uuid = guid.str();

	aiColor3D color(1.f, 1.f, 1.f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ComponentMaterial materialComponent(color.r, color.g, color.b, 1.0f);
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
	
	sprintf_s(materialComponent.texturePath, savePath.c_str());

	if (!App->fileSystem->Write(materialPath, &materialComponent, sizeof(materialComponent)))
	{
		LOG("Failed importing %s", materialPath.c_str());		
		return "";
	}
	GameObject* gameObject = new GameObject("Material");
	gameObject->gameObjectType = GameObject::GameObjectType::MATERIAL_CONTAINER;
	gameObject->InsertComponent(&materialComponent);
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	sprintf_s(gameObject->path, materialPath.c_str());
	gameObject->Serialize(writer);	
	materialPath = "Library/Materials/" + uuid + "_" + std::to_string(index) + ".mgo";
	unsigned dataSize = strlen(sb.GetString());
	char* serialData = new char[dataSize];
	memcpy(serialData, sb.GetString(), dataSize);
	App->fileSystem->Write(materialPath.c_str(), serialData, dataSize);
	RELEASE(serialData);
	return materialPath;
}

ComponentMaterial* MaterialImporter::Load(char path[4096])
{
	ComponentMaterial mat(1.f, 1.f, 1.f, 1.f);
	if (!App->fileSystem->Load(std::string(path), &mat, sizeof(mat)))
	{
		LOG("Couldn't read material %s", path);
		return nullptr;
	}
	
	//TODO:Create a Cloner
	ComponentMaterial* newMat = new ComponentMaterial(mat.color.x, mat.color.y, mat.color.z, mat.color.w); 
	newMat->ambient = mat.ambient;
	newMat->diffuse = mat.diffuse;
	newMat->specular = mat.specular;
	sprintf_s(newMat->instanceOf, mat.instanceOf);
	sprintf_s(newMat->name, mat.name);
	newMat->program = mat.program;
	sprintf_s(newMat->texturePath, mat.texturePath);
	if (strlen(newMat->texturePath) > 0) {
		newMat->texture = App->textures->Load(newMat->texturePath);
	}

	return newMat;
}

void MaterialImporter::Save(char path[1024], ComponentMaterial * material)
{
	if (!App->fileSystem->Write(std::string(path), material, sizeof(*material)))
	{
		LOG("Failed saving material %s", path);
	}
}

