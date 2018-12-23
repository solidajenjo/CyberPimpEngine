#ifndef _MATERIAL_IMPORTER_H
#define _MATERIAL_IMPORTER_H

#include "MathGeoLib/include/Math/float4.h"

class ComponentMaterial;
class GameObject;

struct aiMaterial;

class MaterialImporter
{
public:
	struct MaterialData
	{
		float4 diffuseColor = float4(1.f, 1.f, 1.f, 1.f);
		float4 specularColor = float4(1.f, 1.f, 1.f, 1.f);
		float4 emissiveColor = float4(0.f, 0.f, 0.f, 0.f);

		float kDiffuse = 0.f, kAmbient = 0.f, kSpecular = 0.f, shininess = 64.f;

		char materialPath[1024] = "";

		char diffuseMap[1024] = "";
		char normalMap[1024] = "";
		char specularMap[1024] = "";
		char occlusionMap[1024] = "";
		char emissiveMap[1024] = "";
	};

	std::string Import(const aiMaterial * material, GameObject* &materialImported) const; //stores texture as DDS in engine folders return path of the material & a gameobject with the material by reference	
	void Import(const char path[1024]) const;
	ComponentMaterial* Load(const char path[1024]) const;
	void Save(const char path[1024], const ComponentMaterial* material) const;
};

#endif 