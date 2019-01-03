#ifndef _COMPONENT_LIGHT_H_
#define _COMPONENT_LIGHT_H_

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"

class ComponentLight : public Component
{
public:

	enum class LightTypes
	{
		DIRECTIONAL = 0,
		POINT,
		SPOT
	};

	ComponentLight() : Component(ComponentTypes::LIGHT_COMPONENT) {}

	~ComponentLight();

	void EditorDraw() override;

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	void UnSerialize(rapidjson::Value &value) override;
	ComponentLight* Clone() override;

	//members

	LightTypes lightType = LightTypes::POINT;
	float3 color = float3::one;
	float3 attenuation = float3(.001f, .05f, .1f); // 0 - Constant | 1 - Linear | 2 - Quadric
	float innerAngle = .3f;
	float outterAngle = .8f;
};

#endif