#ifndef _MODULE_CAMERA_H
#define _MODULE_CAMERA_H

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class ModuleCamera :
	public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;

	void yaw(float amount);
	void pitch(float amount);
	void LookAt(float3 target, float3 eye);

//members

	math::float3 camPos = float3::zero;

	math::float4x4 proj = float4x4::identity;
	math::float4x4 view = float4x4::identity;

	math::float3 forward = float3::zero;
	math::float3 right = float3::zero;
	math::float3 up = float3::zero;

	float vFov = 90, oldFOV = 90, zNear = 0.1f, oldZNear = 0.1f, zFar = 100.f, oldZFar = 100.f, pitchAmount = 0.f;
	float zoomSpeed = 0.1f, moveSpeed = 0.1f , rotSpeed = 0.01f;
	float oldAspectRatio = 0;

};

#endif;

