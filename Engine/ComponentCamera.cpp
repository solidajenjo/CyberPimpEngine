#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "MathGeoLib/include/Math/MathConstants.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "imgui/imgui.h"
#include "GameObject.h"


ComponentCamera::ComponentCamera(bool mainCamera) : Component("Camera")
{
	if (mainCamera)
		App->scene->sceneCamera = this;
}

void ComponentCamera::EditorDraw()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(type))
	{
		if (ImGui::Checkbox("Main Camera", &mainCamera))
		{
			if (App->scene->sceneCamera != nullptr)
			{
				App->scene->sceneCamera->mainCamera = false;
			}
			if (mainCamera)
			{
				App->scene->sceneCamera = this; 
			}
			else
			{
				App->scene->sceneCamera = nullptr;
			}
		}
		ImGui::SameLine();
		ImGui::Checkbox("SkyBox", &useSkyBox);
		ImGui::SliderFloat("FOV", &vFov, 10, 180);
		ImGui::SliderFloat("Z Near", &zNear, 0.01f, 2000.f);
		ImGui::SliderFloat("Z Far", &zFar, 0.01f, 2000.f);
		ImGui::Separator();		
	}
	ImGui::PopID();
}

void ComponentCamera::RecalculateFrustum()
{
	RecalculateFrustum(frustum.front, frustum.up);
	
}

void ComponentCamera::RecalculateFrustum(float3 front, float3 up)
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

void ComponentCamera::Update()
{
	camPos = owner->transform->getGlobalPosition();
	RecalculateFrustum(owner->transform->front, owner->transform->up);
}

void ComponentCamera::yaw(float amount)
{

	Quat rotMat = math::Quat::RotateY(amount);
	RecalculateFrustum(rotMat * frustum.front, rotMat * frustum.up);

}

void ComponentCamera::pitch(float amount)
{

	Quat rotMat = math::Quat::RotateAxisAngle(frustum.WorldRight(), amount);
	RecalculateFrustum(rotMat * frustum.front, rotMat * frustum.up);

}


