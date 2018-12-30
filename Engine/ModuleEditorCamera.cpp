#include "ModuleEditorCamera.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "SDL/include/SDL_keycode.h"
#include "SDL/include/SDL_mouse.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Transform.h"
#include "SubModuleEditorViewPort.h"
#include "ModuleTime.h"



bool ModuleEditorCamera::Init()
{
	editorCamera.zFar = 20000.f;
	editorCamera.RecalculateFrustum(-float3::unitZ, float3::unitY);
	return true;
}

update_status ModuleEditorCamera::Update()
{	
	if (!App->editor->viewPort->cursorIn || guizmoLock)
		return UPDATE_CONTINUE;

	if (App->scene->selected != nullptr && App->scene->selected->aaBB != nullptr)
		editorCamera.target = App->scene->selected->aaBBGlobal->CenterPoint();

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		float movementScale = 1.f;

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
			movementScale = 2.f;
		}
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			editorCamera.camPos = editorCamera.camPos + float3(0.f, App->appScale * movementScale * editorCamera.moveSpeed * App->appTime->realDeltaTime, 0.f);
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		{
			editorCamera.camPos = editorCamera.camPos + float3(0.f, App->appScale * -editorCamera.moveSpeed * movementScale * App->appTime->realDeltaTime, 0.f);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			editorCamera.camPos = editorCamera.camPos - editorCamera.frustum.WorldRight() * App->appScale *  editorCamera.moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			editorCamera.camPos = editorCamera.camPos + editorCamera.frustum.WorldRight() * App->appScale *  editorCamera.moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			editorCamera.camPos = editorCamera.camPos + editorCamera.frustum.front * App->appScale * editorCamera.moveSpeed * movementScale * App->appTime->realDeltaTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			editorCamera.camPos = editorCamera.camPos - editorCamera.frustum.front * App->appScale * editorCamera.moveSpeed * movementScale * App->appTime->realDeltaTime;
		}

		iPoint mouseMotion = App->input->GetMouseMotion();

		editorCamera.pitch(-mouseMotion.y * editorCamera.rotSpeed * App->appTime->realDeltaTime);
		editorCamera.yaw(-mouseMotion.x * editorCamera.rotSpeed * App->appTime->realDeltaTime);
		editorCamera.RecalculateFrustum();
	}

	

	bool focus = false;
	float orbitFocus = 0.f;
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
	{
		focusLerp = 0.f;
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{			

		iPoint mouseMotion = App->input->GetMouseMotion();
		Quat orbitMat = Quat::RotateY(mouseMotion.x * editorCamera.rotSpeed * 0.5f * App->appTime->realDeltaTime) * math::Quat::RotateAxisAngle(editorCamera.frustum.WorldRight(), mouseMotion.y * editorCamera.rotSpeed * 0.5f * App->appTime->realDeltaTime);
		editorCamera.camPos =  orbitMat * (editorCamera.camPos - editorCamera.target) + editorCamera.target;
		editorCamera.RecalculateFrustum();
		focus = true;

		float angleToTarget = RadToDeg(editorCamera.frustum.front.AngleBetween(editorCamera.target - editorCamera.camPos)); 
		if (focusLerp > 1.0f && angleToTarget <= 1.0f)
		{			
			focusLerp = 1.f;
		}
		else
		{
			orbitFocus = 0.1f;
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		focusLerp = 0.f;
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT || focus)
	{
		if (focusLerp < 1.f)
			focusLerp += 0.05f + orbitFocus;

		if ((editorCamera.target - editorCamera.camPos).AngleBetween(editorCamera.frustum.front) > 0.f) //should rotate?
		{
			Quat currentLookingQuat = Quat::LookAt(editorCamera.frustum.front, (editorCamera.frustum.front * 2.f).Normalized(), editorCamera.frustum.up, float3::unitY); //curent quat
			Quat lookMat = Quat::LookAt(editorCamera.frustum.front, (editorCamera.target - editorCamera.camPos).Normalized(), editorCamera.frustum.up, float3::unitY); //target rotation
			lookMat = currentLookingQuat.Lerp(lookMat, focusLerp); //smooth look at
			editorCamera.RecalculateFrustum(lookMat * editorCamera.frustum.front, lookMat * editorCamera.frustum.up);
		}
	}
	if (App->input->wheelAmount != 0)
	{
		
		editorCamera.camPos = editorCamera.camPos + editorCamera.frustum.front * editorCamera.zoomSpeed * App->input->wheelAmount * App->appTime->realDeltaTime;
		editorCamera.RecalculateFrustum();
	}


	return UPDATE_CONTINUE;
}




