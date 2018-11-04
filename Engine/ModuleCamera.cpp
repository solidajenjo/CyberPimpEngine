#include "ModuleCamera.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"


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
	camPos.y = 5;
	camPos.z = 10;
	float3 target(0,0,0);
	float3 eye(camPos);
	
	up = float3(0, 1, 0);
	LookAt(target, eye);
	return true;
}

void ModuleCamera::LookAt(float3 target, float3 eye) 
{
	forward = math::float3(target - eye); forward.Normalize();
	right = math::float3(forward.Cross(up)); right.Normalize();
	up = math::float3(right.Cross(forward));

	view[0][0] = right.x; view[0][1] = right.y; view[0][2] = right.z;
	view[1][0] = up.x; view[1][1] = up.y; view[1][2] = up.z;
	view[2][0] = -forward.x; view[2][1] = -forward.y; view[2][2] = -forward.z;
	view[0][3] = -right.Dot(camPos); view[1][3] = -up.Dot(camPos); view[2][3] = forward.Dot(camPos);
	view[3][0] = 0; view[3][1] = 0; view[3][2] = 0; view[3][3] = 1;

	pitchAmount = forward.AngleBetweenNorm(float3(forward.x, 0.f, forward.z).Normalized());
}


update_status ModuleCamera::PreUpdate()
{

	float aspect = (float)App->renderer->viewPortWidth / (float)App->renderer->viewPortHeight;
	if (aspect != oldAspectRatio || vFov != oldFOV || zFar != oldZFar || zNear != oldZNear) //projection changes -> update
	{
		Frustum frustum;
		frustum.type = FrustumType::PerspectiveFrustum;
		frustum.pos = float3::zero;
		frustum.front = -float3::unitZ;
		frustum.up = float3::unitY;
		frustum.nearPlaneDistance = zNear;
		frustum.farPlaneDistance = zFar;
		frustum.verticalFov = (vFov * math::pi / 2) / 180.f;
		frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) *aspect);
		proj = frustum.ProjectionMatrix();
		oldAspectRatio = aspect;
		oldFOV = vFov;
		oldZFar = zFar;
		oldZNear = zNear;
		App->renderer->RecalcFrameBufferTexture();


	}
	return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			camPos = camPos + right * moveSpeed;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			camPos = camPos - right * moveSpeed;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			camPos = camPos + forward * moveSpeed;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			camPos = camPos - forward * moveSpeed;
		}

		iPoint mouseMotion = App->input->GetMouseMotion();
		pitch(-mouseMotion.y * rotSpeed);
		yaw(mouseMotion.x * rotSpeed);
		LookAt(camPos + forward, camPos);
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{
		iPoint mouseMotion = App->input->GetMouseMotion();
		Quat yawRotMat = Quat::RotateY(-mouseMotion.x * rotSpeed);
		camPos = yawRotMat * camPos;
		forward = yawRotMat * forward;
		up = yawRotMat * up;
		
		Quat pitchRotMat = Quat::RotateAxisAngle(right, mouseMotion.y * rotSpeed * up.AngleBetweenNorm(float3(up.x, 0.f, up.z).Normalized())); // rotSpeed * up.AngleBetweenNorm(float3(up.x, 0.f, up.z).Normalized()) reduce rotation speed when near the vertical
		float3 newUp = pitchRotMat * up;
		if (newUp.AngleBetweenNorm(float3(newUp.x, 0.f, newUp.z).Normalized()) > 0.1f) //angle between the new up & his XZ projection
		{
			up = newUp;
			camPos = pitchRotMat * camPos;
			forward = pitchRotMat * forward;
		}
		LookAt(float3(0.f,0.f,0.f), camPos); //TODO: Get geometry center

		//TODO: Roll control 
	}

	if (App->input->wheelAmount != 0)
	{
		camPos = camPos + forward * zoomSpeed * App->input->wheelAmount;
		LookAt(camPos + forward, camPos);
	}

	

	return UPDATE_CONTINUE;
}


void ModuleCamera::yaw(float amount)
{
	Quat rotMat = math::Quat::RotateY(amount);
	forward = rotMat * forward;
	up = rotMat * up;
}

void ModuleCamera::pitch(float amount)
{
	Quat rotMat = math::Quat::RotateAxisAngle(right, amount);
	float3 newForward = rotMat * forward;
	float newPitch = newForward.AngleBetweenNorm(float3(newForward.x, 0.f, newForward.z).Normalized()); //angle between the new forward & his XZ projection
	if (newPitch < 1.2f)
	{
		forward = newForward;
		up = rotMat * up;
		pitchAmount = newPitch;
	}
}


