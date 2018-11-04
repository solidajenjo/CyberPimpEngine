#ifndef _MODULE_CAMERA_H
#define _MODULE_CAMERA_H

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

class ModuleCamera :
	public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init();
	update_status PreUpdate();
	update_status Update();

	void yaw(float amount);
	void pitch(float amount);
	void LookAt(float3 target, float3 eye);

	math::float3 camPos;

	math::float4x4 proj;
	math::float4x4 view;

	math::float3 forward;
	math::float3 right;
	math::float3 up;

	float vFov = 90, oldFOV = 90, zNear = 0.1f, oldZNear = 0.1f, zFar = 100.f, oldZFar = 100.f, pitchAmount;
	float zoomSpeed = 0.1f, moveSpeed = 0.1f , rotSpeed = 0.01f;
	float oldAspectRatio = 0;

};

#endif;

