#include "ModuleEditorCamera.h"
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
#include "ModuleWindow.h"
#include "imgui/imgui.h"



bool ModuleEditorCamera::Init()
{
	editorCamera.zFar = 20000.f;
	editorCamera.RecalculateFrustum(-float3::unitZ, float3::unitY);
	return true;
}

update_status ModuleEditorCamera::Update()
{
	if (outviewPortWait > 0.f)
	{
		outviewPortWait -= App->appTime->realDeltaTime; 
		return UPDATE_CONTINUE;
	}
	if (App->scene->selected != nullptr && !App->scene->IsRoot(App->scene->selected))
		editorCamera.target = App->scene->selected->aaBBGlobal->CenterPoint();
	else if (App->scene->selected == nullptr)
		editorCamera.target = editorCamera.camPos + (editorCamera.frustum.front * App->appScale * 10.f);
	LOG("%.3f %.3f %.3f", editorCamera.target.x, editorCamera.target.y, editorCamera.target.z);
	int winX, winY, winSizeX, winSizeY;

	SDL_GetWindowPosition(App->window->window, &winX, &winY);
	SDL_GetWindowSize(App->window->window, &winSizeX, &winSizeY);
	int xPos, yPos;
	SDL_GetMouseState(&xPos, &yPos);
		
	if (isOrbiting || isRotating)
	{
		if (xPos < 0)
		{
			SDL_WarpMouseInWindow(App->window->window, winSizeX, yPos);
			App->input->mouse_motion.x = 0;
			App->input->mouse_motion.y = 0;
			outviewPortWait = .05F;
			return UPDATE_CONTINUE;
		}
		if (xPos > winSizeX)
		{
			SDL_WarpMouseInWindow(App->window->window, 0, yPos);
			App->input->mouse_motion.x = 0;
			App->input->mouse_motion.y = 0;
			outviewPortWait = .05F;
			return UPDATE_CONTINUE;
		}
	}
	
	if (!isOrbiting && App->editor->viewPort->isFocused && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		isRotating = true;
	}
	if (!App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		isRotating = false;
	}
	iPoint mouseMotion = App->input->GetMouseMotion();
	if (isRotating || isOrbiting)
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

		editorCamera.pitch(-mouseMotion.y * editorCamera.rotSpeed * App->appTime->realDeltaTime);
		editorCamera.yaw(-mouseMotion.x * editorCamera.rotSpeed * App->appTime->realDeltaTime);
		editorCamera.RecalculateFrustum();



		bool focus = false;
		float orbitFocus = 0.f;
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
		{
			focusLerp = 0.f;
		}

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
		{
			isOrbiting = true;
			isRotating = false;
		}
		else
			isOrbiting = false;

		if (isOrbiting)
		{			
			iPoint mouseMotion = App->input->GetMouseMotion();
			Quat orbitMat = Quat::RotateY(mouseMotion.x * editorCamera.rotSpeed * App->appTime->realDeltaTime) * math::Quat::RotateAxisAngle(editorCamera.frustum.WorldRight(), mouseMotion.y * editorCamera.rotSpeed * App->appTime->realDeltaTime);
			editorCamera.camPos = orbitMat * (editorCamera.camPos - editorCamera.target) + editorCamera.target;
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
		if (App->editor->viewPort->isFocused)
		{
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

				editorCamera.camPos = editorCamera.camPos + editorCamera.frustum.front * editorCamera.zoomSpeed * App->input->wheelAmount * App->appTime->realDeltaTime * App->appScale;
				editorCamera.RecalculateFrustum();
			}
		}
	}
	return UPDATE_CONTINUE;
}




