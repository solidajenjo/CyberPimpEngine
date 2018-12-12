#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "Component.h"
#include "MathGeoLib/include/Math/float4.h"
#include <string>
#include <map>

class ComponentMap;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(float r, float g, float b, float a);
	~ComponentMaterial();

	void EditorDraw() override;
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	bool Release() override;

//members

	ComponentMap* texture = nullptr;
	ComponentMap* normal = nullptr;
	ComponentMap* specular = nullptr;
	ComponentMap* occlusion = nullptr;
	ComponentMap* emissive = nullptr;

	unsigned program = 0;
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

	//Resource management
	static ComponentMaterial* GetMaterial(const std::string path);
	static std::map<std::string, ComponentMaterial*> materialsLoaded;
};

#endif