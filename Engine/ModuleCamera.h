#ifndef _MODULE_CAMERA_H
#define _MODULE_CAMERA_H

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleCamera :
	public Module
{
public:

	bool Init() override;
	update_status Update() override;

	void yaw(float amount);
	void pitch(float amount);
	void RecalculateFrustum(); //window events updater
	void RecalculateFrustum(float3 front, float3 up);

//members

	math::float3 camPos = float3(0.f, 1.f, 10.f);

	float vFov = 90.f, zNear = 0.1f, zFar = 400.f;
	float zoomSpeed = 15.f, moveSpeed = 5.f , rotSpeed = 0.2f;
	float focusLerp = 0.f;
	float aspectRatio = 1.f;
	float3 target = float3(0.f, 0.f, 0.f);

	Frustum frustum;
};

#endif;

