#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"
#include <string>
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/Math/float3.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera() : Component(ComponentTypes::CAMERA_COMPONENT) {}

	ComponentCamera(bool mainCamera);

	void EditorDraw() override;

	void Update();
	void yaw(float amount);
	void pitch(float amount);
	void RecalculateFrustum(); //window events updater
	void RecalculateFrustum(float3 front, float3 up);
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	void UnSerialize(rapidjson::Value &value) override;

	//members

	math::float3 camPos = float3(0.f, 1.f, 10.f);

	float vFov = 90.f, zNear = 1.f, zFar = 2000.f;
	float zoomSpeed = 15.f, moveSpeed = 10.f, rotSpeed = 0.2f;
	float aspectRatio = 1.f;
	float3 target = float3(0.f, 0.f, 0.f);

	Frustum frustum;

	bool mainCamera = false;
	bool useSkyBox = true;
	
};

#endif