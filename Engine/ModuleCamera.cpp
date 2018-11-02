#include "ModuleCamera.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"


ModuleCamera::ModuleCamera()
{
}


ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	//View
	camPos.x = 0;
	camPos.y = 0;
	camPos.z = 10;
	float3 target(0,0,0);
	float3 eye(camPos);
	
	up = float3(0, 1, 0);
	forward = math::float3(target - eye); forward.Normalize();
	right = math::float3(forward.Cross(up)); right.Normalize();
	up = math::float3(right.Cross(forward));

	view[0][0] = right.x; view[0][1] = right.y; view[0][2] = right.z;
	view[1][0] = up.x; view[1][1] = up.y; view[1][2] = up.z;
	view[2][0] = -forward.x; view[2][1] = -forward.y; view[2][2] = -forward.z;
	view[0][3] = -right.Dot(eye); view[1][3] = -up.Dot(eye); view[2][3] = forward.Dot(eye);
	view[3][0] = 0; view[3][1] = 0; view[3][2] = 0; view[3][3] = 1;

	//Perspective
	float aspect = App->window->screenWidth / App->window->screenHeight;

	Frustum frustum;
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) *aspect);
	proj = frustum.ProjectionMatrix();

	camPos.x = 0;
	camPos.y = 0;
	camPos.z = 0;

	return true;
}

update_status ModuleCamera::Update()
{

	view[0][0] = right.x; view[0][1] = right.y; view[0][2] = right.z;
	view[1][0] = up.x; view[1][1] = up.y; view[1][2] = up.z;
	view[2][0] = -forward.x; view[2][1] = -forward.y; view[2][2] = -forward.z;
	view[0][3] = -right.Dot(camPos); view[1][3] = -up.Dot(camPos); view[2][3] = forward.Dot(camPos);
	view[3][0] = 0; view[3][1] = 0; view[3][2] = 0; view[3][3] = 1;

	float aspect = App->window->screenWidth / App->window->screenHeight;

	Frustum frustum;
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = (vFov * math::pi/2) / 180.f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) *aspect);
	proj = frustum.ProjectionMatrix();
	
	glBegin(GL_TRIANGLES);
	glVertex3f(-1, -1, 0);
	glVertex3f(1, -1, 0);
	glVertex3f(0, 1, 0);
	glEnd();
	return UPDATE_CONTINUE;
}

void ModuleCamera::rotate(float xRot, float yRot, float zRot)
{
	float4x4 rotMat = float4x4::FromQuat(math::Quat::FromEulerXYZ(xRot, yRot, zRot));
	float4 newForward = rotMat * float4(forward, 1.0f);
	forward = newForward.xyz().Normalized();
	right = math::float3(forward.Cross(up)); right.Normalize();
	up = math::float3(right.Cross(forward)).Normalized();
}


void ModuleCamera::recalcProjection(float hFOV, float zNear, float zFar)
{
	//MULTIPLICAR AL REVES LA ROTATION MATRIX
}
