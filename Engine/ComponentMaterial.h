#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include "Component.h"
#include "MathGeoLib/include/Math/float4.h"
#include <string>
#include <list>

class GameObject;
class ComponentMesh;
class Writer;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(float r, float g, float b, float a);
	~ComponentMaterial();

	void EditorDraw() override;
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	void UnSerialize(rapidjson::Value &value) override;

//members

	unsigned texture = 0;
	unsigned program = 0;
	float4 color = float4(1.f, 1.f, 1.f, 1.f);
	float diffuse = 0.f, ambient = 0.f, specular = 0.f;
	char name[40] = "";
	char instanceOf[40] = "";
	char texturePath[1024] = "";
	char materialPath[1024] = "";
};

#endif