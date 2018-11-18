#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"
#include <string>
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/Math/float3.h"

class GameObject;

class ComponentCamera : public Component
{
public:
	ComponentCamera() : Component("Camera") {}

	ComponentCamera(bool mainCamera);

	void EditorDraw() override;

	void Update();
	void yaw(float amount);
	void pitch(float amount);
	void RecalculateFrustum(); //window events updater
	void RecalculateFrustum(float3 front, float3 up);

	//members

	math::float3 camPos = float3(0.f, 1.f, 10.f);

	float vFov = 90.f, zNear = 0.1f, zFar = 2000.f;
	float zoomSpeed = 15.f, moveSpeed = 20.f, rotSpeed = 0.2f;
	float aspectRatio = 1.f;
	float3 target = float3(0.f, 0.f, 0.f);

	Frustum frustum;

	GameObject* owner = nullptr;

	bool mainCamera = false;
	
};

#endif