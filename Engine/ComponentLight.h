#ifndef _COMPONENT_LIGHT_H_
#define _COMPONENT_LIGHT_H_

#include "Component.h"
#include "Application.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/Sphere.h"

class ComponentLight : public Component
{
public:

	enum class LightTypes
	{
		DIRECTIONAL = 0,
		POINT,
		SPOT
	};

	ComponentLight();

	void EditorDraw() override;

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	void UnSerialize(rapidjson::Value &value) override;
	ComponentLight* Clone() override;

	//members

	LightTypes lightType = LightTypes::POINT;
	float3 color = float3::one;
	//point lights 
	float3 attenuation = float3(1.f, .0009f, .0003f); // 0 - Constant | 1 - Linear | 2 - Quadric
	float influence = .1f;
	float innerAngle = .3f;
	float outterAngle = .8f;

	Sphere pointSphere;

private:

	inline void CalculateSphereRadius()
	{
		float a = influence * attenuation[2];
		float b = influence * attenuation[1];
		float c = (influence * attenuation[0]) - 1.f;
		float p = (-b + sqrt((b * b - 4 * a * c)) / (2 * a));
		float n = (-b - sqrt((b * b - 4 * a * c)) / (2 * a));

		pointSphere.r = abs(p);
	};
};

#endif