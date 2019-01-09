#ifndef _COMPONENT_LIGHT_H_
#define _COMPONENT_LIGHT_H_

#include "Component.h"
#include "Application.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/Sphere.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "Transform.h"
#include "GameObject.h"

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

	bool ConeContainsAABB(const AABB &aabb) const;  // returns true if the aabb is inside or intersects the spot light cone

	inline void CalculateGuizmos()
	{

		//point light

		float a = influence * attenuation[2];
		float b = influence * attenuation[1];
		float c = (influence * attenuation[0]) - 1.f;
		float p = (-b + sqrt((b * b - 4.f * a * c)) / (2.f * a));

		pointSphere.r = abs(p);
		pointSphere.pos = owner->transform->getGlobalPosition();

		//spot light

		spotDistance = pointSphere.r; // max distance is equivalent as the point light radius
		spotOutterDistance = abs(spotDistance / sin(spotDistance));
		spotEndRadius = spotDistance * tan(outterAngle);
	};

	//members

	LightTypes lightType = LightTypes::POINT;
	float3 color = float3::one;
	//point lights 
	float3 attenuation = float3(1.f, .0009f, .0003f); // 0 - Constant | 1 - Linear | 2 - Quadric
	float influence = .1f;
	float innerAngle = .3f;
	float outterAngle = .8f;
	float spotDistance = .0f;
	float spotOutterDistance = .0f;
	float spotEndRadius = .0f;	
	 
	Sphere pointSphere;

};

#endif