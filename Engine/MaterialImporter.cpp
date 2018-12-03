#include <assert.h>
#include "Application.h"
#include "MaterialImporter.h"
#include "Assimp/include/assimp/material.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"
#include "DevIL/include/IL/ilu.h"
#include "ComponentMaterial.h"
#include "ModuleTextures.h"
#include "SDL/include/SDL_rwops.h"
#include "crossguid/include/crossguid/guid.hpp"

std::string MaterialImporter::Import(const aiMaterial* material, const unsigned index)
{
	assert(material != nullptr);
	xg::Guid guid = xg::newGuid();
	std::string uuid = guid.str();

	aiColor3D color(1.f, 1.f, 1.f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ComponentMaterial materialComponent(color.r, color.g, color.b, 1.0f);
	std::string materialPath = "Library/Materials/" + uuid + "_" + std::to_string(index) + ".dmt";
	
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

	SDL_RWops *rw = SDL_RWFromFile(materialPath.c_str(), "w");
	if (rw == nullptr)
	{
		LOG("Failed importing %s -> %s", materialPath.c_str(), SDL_GetError());
		return "";
	}
	if (SDL_RWwrite(rw, &materialComponent, sizeof(materialComponent), 1) != 1)
	{
		LOG("Failed importing %s -> %s", materialPath.c_str(), SDL_GetError());		
		SDL_RWclose(rw);
		return "";
	}
	SDL_RWclose(rw);
	return materialPath;
}

ComponentMaterial* MaterialImporter::Load(char path[4096])
{
	SDL_RWops *rw = SDL_RWFromFile(path, "r");
	if (rw == nullptr)
	{
		LOG("Couldn't read material %s -> %s", path, SDL_GetError());
		return nullptr;
	}
	ComponentMaterial mat(1.f, 1.f, 1.f, 1.f);
	if (SDL_RWread(rw, &mat, sizeof(ComponentMaterial), 1) != 1)
	{
		LOG("Couldn't read material %s -> %s", path, SDL_GetError());
		SDL_RWclose(rw);
		return nullptr;
	}
	SDL_RWclose(rw);
	
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
	SDL_RWops *rw = SDL_RWFromFile(path, "w");
	if (rw == nullptr)
	{
		LOG("Failed saving material %s -> %s", path, SDL_GetError());
		return;
	}
	if (SDL_RWwrite(rw, material, sizeof(*material), 1) != 1)
	{
		LOG("Failed saving material %s -> %s", path, SDL_GetError());
		SDL_RWclose(rw);
		return;
	}
	SDL_RWclose(rw);
}

