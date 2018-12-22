#include "Transform.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include <queue>
#include "imgui/imgui.h"
#include "SDL/include/SDL_mouse.h"

float* Transform::GetModelMatrix()
{	
	return &modelMatrixGlobal[0][0]; 
}

float3 Transform::getGlobalPosition()
{
	return modelMatrixGlobal.Col3(3);
}


Transform::Transform(GameObject* go) : Component(ComponentTypes::TRANSFORM_COMPONENT), owner(go)
{
}

void Transform::Rotate(const float3& rotations) 
{
	modelMatrixLocal = modelMatrixLocal * Quat::FromEulerXYZ(rotations.x, rotations.y, rotations.z); //rotate in local coords
	rotation += rotations;	

	PropagateTransform();

}

void Transform::Translate(const float3& translation)
{	
	position += translation;
	modelMatrixLocal = float4x4::FromTRS(position, float4x4::FromEulerXYZ(rotation.x, rotation.y, rotation.z), scale);
	PropagateTransform();
}

void Transform::Scale(const float3& scalation)
{
	scale += scalation;
	modelMatrixLocal = float4x4::FromTRS(position, float4x4::FromEulerXYZ(rotation.x, rotation.y, rotation.z), scale);
	PropagateTransform();
}

void Transform::SetPosition(const float3& newPosition)
{
	position = newPosition;
	modelMatrixLocal = float4x4::FromTRS(position, float4x4::FromEulerXYZ(rotation.x, rotation.y, rotation.z), scale);
	PropagateTransform();
}

void Transform::SetRotation(const float3& newRotation)
{
	rotation = newRotation;
	modelMatrixLocal = float4x4::FromTRS(position, float4x4::FromEulerXYZ(rotation.x, rotation.y, rotation.z), scale);
	PropagateTransform();
}

void Transform::SetScale(const float3& newScale)
{
	scale = newScale;
	modelMatrixLocal = float4x4::FromTRS(position, float4x4::FromEulerXYZ(rotation.x, rotation.y, rotation.z), scale);
	PropagateTransform();
}

void Transform::SetTransform(const float3& pos, const float3& rot, const float3& scl)
{
	position = pos;
	rotation = rot;
	scale = scl;	
	if (owner == nullptr)
		return;
	RecalcModelMatrix();	
	PropagateTransform();
}


void Transform::ExtractLocalTransformFromMatrix()
{
	float3x3 rot;
	modelMatrixLocal.Decompose(position, rot, scale);
	rotation = rot.ToEulerXYZ();
}

void Transform::RecalcModelMatrix()
{	
	rotQuat = Quat::FromEulerXZY(rotation.x, rotation.z, rotation.y);

	modelMatrixLocal = float4x4::FromTRS(position, rotQuat.ToFloat4x4(), scale);

	if (owner->parent != nullptr)
		modelMatrixGlobal = owner->parent->transform->modelMatrixGlobal.Mul(modelMatrixLocal); //set relative to parent transform
	else
		modelMatrixGlobal = modelMatrixLocal; //no parent

	PropagateTransform();

}

void Transform::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("position"); 
	writer.StartObject(); 
	writer.String("x"); writer.Double(position.x);
	writer.String("y"); writer.Double(position.y);
	writer.String("z"); writer.Double(position.z);
	writer.EndObject();

	writer.String("rotation");
	writer.StartObject();
	writer.String("x"); writer.Double(rotation.x);
	writer.String("y"); writer.Double(rotation.y);
	writer.String("z"); writer.Double(rotation.z);
	writer.EndObject();

	writer.String("scale");
	writer.StartObject();
	writer.String("x"); writer.Double(scale.x);
	writer.String("y"); writer.Double(scale.y);
	writer.String("z"); writer.Double(scale.z);
	writer.EndObject();

	writer.EndObject();
}

void Transform::UnSerialize(rapidjson::Value & value)
{
	position.x = value["position"]["x"].GetDouble();
	position.y = value["position"]["y"].GetDouble();
	position.z = value["position"]["z"].GetDouble();
	
	rotation.x = value["rotation"]["x"].GetDouble();
	rotation.y = value["rotation"]["y"].GetDouble();
	rotation.z = value["rotation"]["z"].GetDouble();
	
	scale.x = value["scale"]["x"].GetDouble();
	scale.y = value["scale"]["y"].GetDouble();
	scale.z = value["scale"]["z"].GetDouble();
	
	RecalcModelMatrix();
}

void Transform::EditorDraw()
{
	GameObject* selected = App->scene->selected;
	ImGui::Text(selected->name);
	float3 rot = RadToDeg(rotation);
	float3 pos = float3(position);
	float3 scl = float3(scale);

	static float3 oldPos = pos;
	static float3 oldRot = rot;
	static float3 oldScl = scl;
	static bool windowJumped = false;
	static bool jumpRefresh = false;
	if (jumpRefresh) //it tooks 2 frames to change the value with the new pos after a big jump
	{
		//restore values after jump
		pos = oldPos;
		rot = oldRot;
		scl = oldScl;
		jumpRefresh = false;
	}
	if (windowJumped) //transition state when jumped && before imgui updates new value with mouse position
	{
		oldPos = pos;
		oldRot = rot;
		oldScl = scl;
		windowJumped = false;
		jumpRefresh = true;
	}

	ImGui::PushID(1);
	if (ImGui::DragFloat3("Position (Relative)", &pos.x, 0.01f))
	{		
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 winPos = ImGui::GetMousePos();
		ImVec2 winBegin = ImGui::GetWindowPos();
		ImVec2 winEnd = ImVec2(winBegin.x + size.x, winBegin.y + size.y);
		if (winPos.x < winBegin.x)
		{
			SDL_WarpMouseInWindow(App->window->window, winEnd.x, winPos.y);
			windowJumped = true;
		}
		if (winPos.x > winEnd.x)
		{
			SDL_WarpMouseInWindow(App->window->window, winBegin.x, winPos.y);
			windowJumped = true;
		}
		if (!windowJumped && !jumpRefresh)
			SetPosition(pos);
	}
	ImGui::PopID();
	ImGui::PushID(2);
	if (ImGui::DragFloat3("Rotation (Local)", &rot.x, 0.1f))
	{
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 winPos = ImGui::GetMousePos();
		ImVec2 winBegin = ImGui::GetWindowPos();
		ImVec2 winEnd = ImVec2(winBegin.x + size.x, winBegin.y + size.y);
		if (winPos.x < winBegin.x)
		{
			SDL_WarpMouseInWindow(App->window->window, winEnd.x, winPos.y);
			windowJumped = true;
		}
		if (winPos.x > winEnd.x)
		{
			SDL_WarpMouseInWindow(App->window->window, winBegin.x, winPos.y);
			windowJumped = true;
		}
		if (!windowJumped && !jumpRefresh)
			SetRotation(DegToRad(rot));
	}
	ImGui::PopID();
	ImGui::PushID(3);
	if (ImGui::DragFloat3("Scale (Local)", &scl.x, 0.01f))
	{
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 winPos = ImGui::GetMousePos();
		ImVec2 winBegin = ImGui::GetWindowPos();
		ImVec2 winEnd = ImVec2(winBegin.x + size.x, winBegin.y + size.y);
		if (winPos.x < winBegin.x)
		{
			SDL_WarpMouseInWindow(App->window->window, winEnd.x, winPos.y);
			windowJumped = true;
		}
		if (winPos.x > winEnd.x)
		{
			SDL_WarpMouseInWindow(App->window->window, winBegin.x, winPos.y);
			windowJumped = true;
		}
		if (!windowJumped && !jumpRefresh)
			SetScale(scl);
	}
	ImGui::PopID();
}

Transform * Transform::Clone()
{
	Transform* newTrf = new Transform();

	newTrf->position = position;
	newTrf->localPosition = localPosition;
	newTrf->rotation = rotation;
	newTrf->scale = scale;
	
	return newTrf;
}

void Transform::UpdateAABB() const
{
	//update AABB
	if (owner->aaBB != nullptr)
	{
		owner->aaBBGlobal->SetFrom(*owner->aaBB);
		owner->aaBBGlobal->TransformAsAABB(modelMatrixGlobal);
	}
}
void Transform::PropagateTransform() // update & propagate transform matrix
{
	if (owner->parent != nullptr)
	{
		modelMatrixGlobal = owner->parent->transform->modelMatrixGlobal.Mul(modelMatrixLocal);
	}
	else
	{
		modelMatrixGlobal = modelMatrixLocal;
	}

	UpdateAABB();
	front = -modelMatrixGlobal.Col3(2);
	up = modelMatrixGlobal.Col3(1);
	right = modelMatrixGlobal.Col3(0);
	
	//propagate down 
	std::queue<GameObject*> stackGO; //Top to bottom propagation
	stackGO.push(this->owner);

	while (!stackGO.empty())
	{
		GameObject* GO = stackGO.front(); stackGO.pop();
		for (std::list<GameObject*>::iterator it = GO->children.begin(); it != GO->children.end(); ++it)
		{
			if ((*it)->transform != nullptr)
			{
				stackGO.push(*it); //push the children to propagate later
				//propagate transforms
				(*it)->transform->modelMatrixGlobal = (*it)->parent->transform->modelMatrixGlobal.Mul((*it)->transform->modelMatrixLocal);
				(*it)->transform->front = -(*it)->transform->modelMatrixGlobal.Col3(2);
				(*it)->transform->up = (*it)->transform->modelMatrixGlobal.Col3(1);
				(*it)->transform->right = (*it)->transform->modelMatrixGlobal.Col3(0);
				(*it)->transform->UpdateAABB();
			}
		}
	}

}

void Transform::NewAttachment()
{
	modelMatrixLocal = owner->parent->transform->modelMatrixGlobal.Inverted().Mul(modelMatrixGlobal);
	float4x4 rotMat;
	modelMatrixLocal.Decompose(position, rotMat, scale);
	rotation = rotMat.ToEulerXYZ();
	
}
