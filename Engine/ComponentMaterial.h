#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_


#include "Component.h"
#include "MathGeoLib/include/Math/float4.h"
#include <string>

class GameObject;

class ComponentMaterial : public Component
{
public:

	ComponentMaterial(float r, float g, float b, float a);
	void EditorDraw() override;

//members

	GameObject* owner = nullptr;

	std::string name;
	unsigned texture = 0;
	float4 color = float4(1.f, 1.f, 1.f, 1.f);
	char nameBuff[4096];
};

#endif