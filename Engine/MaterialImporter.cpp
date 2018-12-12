#include <assert.h>
#include "Application.h"
#include "MaterialImporter.h"
#include "Assimp/include/assimp/material.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilut.h"
#include "DevIL/include/IL/ilu.h"
#include "ComponentMaterial.h"
#include "ComponentMap.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include "rapidjson-1.1.0/include/rapidjson/error/en.h"

std::string MaterialImporter::Import(const aiMaterial* material, GameObject* &materialImported) const
{
	assert(material != nullptr);
	xg::Guid guid = xg::newGuid();
	std::string uuid = guid.str();

	aiColor3D color(1.f, 1.f, 1.f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ComponentMaterial* materialComponent = new ComponentMaterial(color.r, color.g, color.b, 1.0f);
	std::string materialPath = "Library/Materials/" + uuid + ".mat";
	
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
	
	sprintf_s(materialComponent->diffuseMap, savePath.c_str());
	sprintf_s(materialComponent->materialPath, materialPath.c_str());

	Save(materialPath.c_str(), materialComponent);
	RELEASE(materialComponent);

	materialComponent = ComponentMaterial::GetMaterial(materialPath);
	materialImported->InsertComponent(materialComponent);
		
	return materialPath;
}

ComponentMaterial* MaterialImporter::Load(const char path[4096]) const
{	
	unsigned size = App->fileSystem->Size(path);
	if (size > 0)
	{
		MaterialData matData;
		if (App->fileSystem->Read(path, &matData, sizeof(matData))) 
		{
			ComponentMaterial* newMat = new ComponentMaterial(1.f, 1.f, 1.f, 1.f);
			
			newMat->diffuseColor = matData.diffuseColor;
			newMat->specularColor = matData.specularColor;
			newMat->emissiveColor = matData.emissiveColor;
			newMat->kDiffuse = matData.kDiffuse;
			newMat->kAmbient = matData.kAmbient;
			newMat->kSpecular = matData.kSpecular;
			newMat->shininess = matData.shininess;
			sprintf_s(newMat->diffuseMap, matData.diffuseMap);
			sprintf_s(newMat->materialPath, matData.materialPath);
			sprintf_s(newMat->normalMap, matData.normalMap);
			sprintf_s(newMat->specularMap, matData.specularMap);
			sprintf_s(newMat->occlusionMap, matData.occlusionMap);
			sprintf_s(newMat->emissiveMap, matData.emissiveMap);

			if (strlen(newMat->diffuseMap) > 0)
			{
				newMat->texture = ComponentMap::GetMap(newMat->diffuseMap);
			}

			if (strlen(newMat->normalMap) > 0)
			{
				newMat->normal = ComponentMap::GetMap(newMat->normalMap); 
			}

			if (strlen(newMat->specularMap) > 0)
			{
				newMat->specular = ComponentMap::GetMap(newMat->specularMap); 
			}

			if (strlen(newMat->occlusionMap) > 0)
			{
				newMat->occlusion = ComponentMap::GetMap(newMat->occlusionMap);
			}

			if (strlen(newMat->emissiveMap) > 0)
			{
				newMat->emissive = ComponentMap::GetMap(newMat->emissiveMap); 
			}
			
			return newMat;

		}
	}
	return nullptr;
}

void MaterialImporter::Save(const char path[1024], const ComponentMaterial * material) const
{
	
	MaterialData matData;
	matData.diffuseColor = material->diffuseColor;
	matData.specularColor = material->specularColor;
	matData.emissiveColor = material->emissiveColor;
	matData.kDiffuse = material->kDiffuse;
	matData.kAmbient = material->kAmbient;
	matData.kSpecular = material->kSpecular;
	matData.shininess = material->shininess;
	sprintf_s(matData.diffuseMap, material->diffuseMap);
	sprintf_s(matData.materialPath, material->materialPath);
	sprintf_s(matData.normalMap, material->normalMap);
	sprintf_s(matData.specularMap, material->specularMap);
	sprintf_s(matData.occlusionMap, material->occlusionMap);
	sprintf_s(matData.emissiveMap, material->emissiveMap);
	
	if (!App->fileSystem->Write(std::string(path), &matData, sizeof(matData)))
	{
		LOG("Failed saving material %s", path);
	}
}

