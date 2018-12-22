#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "MathGeoLib/include/Math/MathConstants.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "imgui/imgui.h"
#include "GameObject.h"


ComponentCamera::ComponentCamera(bool mainCamera) : Component(ComponentTypes::CAMERA_COMPONENT)
{
	if (mainCamera)
		App->scene->sceneCamera = this;
}

void ComponentCamera::EditorDraw()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Camera"))
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
		ImGui::SliderFloat("Z Near", &zNear, 0.01f, 20000.f);
		ImGui::SliderFloat("Z Far", &zFar, 0.01f, 20000.f);
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

void ComponentCamera::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("cam_pos"); 
	writer.StartObject();
	writer.String("x"); writer.Double(camPos.x);
	writer.String("y"); writer.Double(camPos.y);
	writer.String("z"); writer.Double(camPos.z);
	writer.EndObject();
	writer.String("main"); writer.Bool(mainCamera);
	writer.String("vFov"); writer.Double(vFov);
	writer.String("zNear"); writer.Double(zNear);
	writer.String("zFar"); writer.Double(zFar);

	writer.EndObject();
}

void ComponentCamera::UnSerialize(rapidjson::Value & value)
{
	camPos.x = value["cam_pos"]["x"].GetDouble();
	camPos.y = value["cam_pos"]["y"].GetDouble();
	camPos.z = value["cam_pos"]["z"].GetDouble();
	mainCamera = value["main"].GetBool();
	if (mainCamera)
	{
		App->scene->sceneCamera = this;
	}
	vFov = value["vFov"].GetDouble();
	zNear = value["zNear"].GetDouble();
	zFar = value["zFar"].GetDouble();
	RecalculateFrustum();
}

ComponentCamera * ComponentCamera::Clone()
{
	ComponentCamera* newCam = new ComponentCamera();
	newCam->camPos = camPos;
	newCam->vFov = vFov;
	newCam->zNear = zNear;
	newCam->zFar = zFar;
	newCam->zoomSpeed = zoomSpeed;
	newCam->moveSpeed = moveSpeed;
	newCam->rotSpeed = rotSpeed;
	newCam->aspectRatio = aspectRatio;
	newCam->target = target;
	newCam->frustum = frustum;
	
	return newCam;
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


