#include "Transform.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include <queue>
#include "imgui/imgui.h"

float* Transform::GetModelMatrix()
{	
	return &modelMatrixGlobal[0][0]; 
}


Transform::Transform(GameObject* go) : Component("Transform"), owner(go)
{
}

void Transform::Rotate(const float3& rotations)
{
	//modelMatrixLocal = Quat::FromEulerXYZ(rotations.x, rotations.y, rotations.z) * modelMatrixLocal; //rotate in parent's coords
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

	PropagateTransform();
}

void Transform::SetRotation(const float3& newRotation)
{
	rotation = newRotation;
	modelMatrixLocal = Quat::FromEulerXZY(rotation.x, rotation.z, rotation.y);
	PropagateTransform();
}

void Transform::SetScale(const float3& newScale)
{
	scale = newScale;

	PropagateTransform();
}

void Transform::SetTransform(const float3& pos, const float3& rot, const float3& scl)
{
	position = pos;
	rotation = rot;
	scale = scl;
	RecalcModelMatrix();
	PropagateTransform();
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

void Transform::EditorDraw()
{
	GameObject* selected = App->scene->selected;
	ImGui::Text(selected->name.c_str());
	float3 rot = RadToDeg(rotation);
	float3 rotFinal = RadToDeg(rotation);
	float3 pos = float3(position);
	float3 scl = float3(scale);
	ImGui::PushID(1);
	if (ImGui::DragFloat3("Position (Relative)", &pos.x, 0.01f))
	{
		Translate(position - pos);
	}
	ImGui::PopID();
	ImGui::PushID(2);
	if (ImGui::DragFloat3("Rotation (Local)", &rotFinal.x, 0.1f))
	{
		Rotate(DegToRad(rot - rotFinal));
	}
	ImGui::PopID();
	ImGui::PushID(3);
	if (ImGui::DragFloat3("Scale (Local)", &scl.x, 0.01f))
	{
		Scale(scale - scl);
	}
	
	ImGui::PopID();
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
			stackGO.push(*it); //push the children to propagate later
			//propagate transforms
			(*it)->transform->modelMatrixGlobal = (*it)->parent->transform->modelMatrixGlobal.Mul((*it)->transform->modelMatrixLocal);
			(*it)->transform->front = -(*it)->transform->modelMatrixGlobal.Col3(2);
			(*it)->transform->up = (*it)->transform->modelMatrixGlobal.Col3(1);
			(*it)->transform->right = (*it)->transform->modelMatrixGlobal.Col3(0);
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
