#include "ModuleCamera.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "SDL/include/SDL_keycode.h"
#include "SDL/include/SDL_mouse.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"


void ModuleCamera::RecalculateFrustum(float3 front, float3 up)
{		
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = camPos;
	frustum.front = front.Normalized();
	frustum.up = up.Normalized();
	float3::Orthonormalize(frustum.front, frustum.up);
	frustum.nearPlaneDistance = zNear;
	frustum.farPlaneDistance = zFar;
	frustum.verticalFov = (vFov * math::pi / 2) / 180.f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspectRatio);		
}


bool ModuleCamera::Init()
{
	RecalculateFrustum(-float3::unitZ, float3::unitY);
	return true;
}

update_status ModuleCamera::Update()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		float movementScale = 1.f;

		target = frustum.front;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
			movementScale = 2.f;
		}
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			camPos = camPos + float3(0.f, movementScale * moveSpeed * App->appTime->realDeltaTime, 0.f);
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		{
			camPos = camPos + float3(0.f, -moveSpeed * movementScale * App->appTime->realDeltaTime, 0.f);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			camPos = camPos + frustum.WorldRight() * moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			camPos = camPos - frustum.WorldRight() * moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			camPos = camPos + frustum.front * moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			camPos = camPos - frustum.front * moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		
		iPoint mouseMotion = App->input->GetMouseMotion();

		if (abs(mouseMotion.y) > 100)
			mouseMotion.y = 50 * (-mouseMotion.y / mouseMotion.y); //keep direction with division

		float pitchAngle = frustum.front.AngleBetween(float3(frustum.front.x, 0.f, frustum.front.z));
		if (pitchAngle < 1.2f) //lock needed?
			pitch(-mouseMotion.y * rotSpeed * App->appTime->realDeltaTime); //not locking
		else
			if (frustum.front.y > 0) // is looking up?
				pitch(-0.01f); //unlock 
			else
				pitch(0.01f); //unlock
		yaw(mouseMotion.x * rotSpeed * App->appTime->realDeltaTime);
		RecalculateFrustum(frustum.front, frustum.up);
	}

	//TODO: Set geometry center as target

	bool focus = false;
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{			
		iPoint mouseMotion = App->input->GetMouseMotion();
		Quat orbitMat = Quat::RotateY(-mouseMotion.x * rotSpeed * App->appTime->realDeltaTime) * math::Quat::RotateAxisAngle(frustum.WorldRight(), mouseMotion.y * rotSpeed * App->appTime->realDeltaTime);
		camPos = target + orbitMat * (camPos - target);
		RecalculateFrustum(frustum.front, frustum.up);
		focus = true;
		if (focusLerp > 1.0f)
			focusLerp = 0.f;
	}
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		focusLerp = 0.f;
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT || focus)
	{
		if (focusLerp < 1.f)
			focusLerp += 0.05f;

		if (focus) //adjust distance with target if needed
		{
			
		}
		if ((target - camPos).AngleBetween(frustum.front) > 0.f) //should rotate?
		{
			Quat currentLookingQuat = Quat::LookAt(frustum.front, (frustum.front * 2.f).Normalized(), frustum.up, float3::unitY); //curent quat
			Quat lookMat = Quat::LookAt(frustum.front, (target - camPos).Normalized(), frustum.up, float3::unitY); //target rotation
			lookMat = currentLookingQuat.Lerp(lookMat, focusLerp); //smooth look at
			RecalculateFrustum(lookMat * frustum.front, lookMat * frustum.up);
		}
	}
	if (App->input->wheelAmount != 0)
	{
		
		camPos = camPos + frustum.front * zoomSpeed * App->input->wheelAmount * App->appTime->realDeltaTime;	
		RecalculateFrustum(frustum.front, frustum.up);
	}


	return UPDATE_CONTINUE;
}


void ModuleCamera::yaw(float amount)
{
	
	Quat rotMat = math::Quat::RotateY(amount);
	RecalculateFrustum(rotMat * frustum.front, rotMat * frustum.up);
	
}

void ModuleCamera::pitch(float amount)
{
	
	Quat rotMat = math::Quat::RotateAxisAngle(frustum.WorldRight(), amount);
	RecalculateFrustum(rotMat * frustum.front, rotMat * frustum.up);

}


