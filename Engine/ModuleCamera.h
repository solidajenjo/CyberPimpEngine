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

	void rotate(float xRot, float yRot, float zRot);

	void recalcProjection(float hFOV, float zNear, float zFar);

	math::float3 camPos;

	math::float4x4 proj;
	math::float4x4 view;

	math::float3 forward;
	math::float3 right;
	math::float3 up;

	float vFov = 90;
	float oldAspectRatio = 0;

};

#endif;

