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


Transform::Transform(GameObject* go) : owner(go)
{
}

void Transform::Rotate(const float3& rotations)
{
	rotQuat = rotQuat * Quat::FromEulerXYZ(rotations.x, rotations.y, rotations.z);
	
	rotation += rotations;	

	PropagateTransform();

}

void Transform::Translate(const float3& translation)
{
	tMat[0][3] += translation.x;
	tMat[1][3] += translation.y;
	tMat[2][3] += translation.z;

	position += translation;

	PropagateTransform();
}

void Transform::Scale(const float3& scalation)
{
	scale += scalation;

	sMat[0][0] = scale.x;
	sMat[1][1] = scale.y;
	sMat[2][2] = scale.z;

	PropagateTransform();
}

void Transform::SetPosition(const float3& newPosition)
{
	position = newPosition;
}

void Transform::SetRotation(const float3& newRotation)
{
	rotation = newRotation;
}

void Transform::SetScale(const float3& newScale)
{
	scale = newScale;
}

void Transform::SetTransform(const float3& pos, const float3& rot, const float3& scl)
{
	
}


void Transform::RecalcModelMatrix()
{	
	tMat[0][3] = position.x;
	tMat[1][3] = position.y;
	tMat[2][3] = position.z;

	rotQuat = Quat::FromEulerXZY(rotation.x, rotation.z, rotation.y);
	//IMGUIZMO
	//TODO: Get Rid of recursivity
	sMat[0][0] *= scale.x;
	sMat[1][1] *= scale.y;
	sMat[2][2] *= scale.z;

	modelMatrixLocal = tMat.Mul(sMat.Mul(rotQuat));

	if (owner->parent != nullptr)
		modelMatrixGlobal = owner->parent->transform->modelMatrixGlobal.Mul(modelMatrixLocal); //set relative to parent transform
	else
		modelMatrixGlobal = modelMatrixLocal; //no parent

	front = modelMatrixGlobal.Row3(3) + float3(0.f, 0.f, 10.f);
	up = modelMatrixGlobal.Row3(3) + float3(0.f, 10.f, 0.f);
	right = modelMatrixGlobal.Row3(3) + float3(10.f, 0.f, 0.f);

	PropagateTransform();

}

void Transform::EditorDraw()
{
	GameObject* selected = App->scene->selected;
	ImGui::Text(selected->name.c_str());
	float3 rot = float3(rotation);
	float3 pos = float3(position);
	float3 scl = float3(scale);
	ImGui::PushID(1);
	if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
	{
		Translate(position - pos);
	}
	ImGui::PopID();
	ImGui::PushID(2);
	if (ImGui::DragFloat3("Rotation", &rot.x, 0.01f))
	{
		Rotate(rotation - rot);
	}
	ImGui::PopID();
	ImGui::PushID(3);
	if (ImGui::DragFloat3("Scale", &scl.x, 0.01f))
	{
		Scale(scale - scl);
	}
	ImGui::PopID();
}

void Transform::PropagateTransform() // update & propagate transform matrix
{	
	modelMatrixLocal = tMat.Mul(sMat.Mul(rotQuat));

	if (owner->parent != nullptr)
	{
		modelMatrixGlobal = owner->parent->transform->modelMatrixGlobal.Mul(modelMatrixLocal);
	}
	else
	{
		modelMatrixGlobal = modelMatrixLocal;
	}

	std::queue<GameObject*> stackGO; //Top to bottom propagation

	for (std::list<GameObject*>::iterator it = owner->children.begin(); it != owner->children.end(); ++it)
		stackGO.push(*it); //push the children to propagate

	while (!stackGO.empty())
	{
		GameObject* GO = stackGO.front(); stackGO.pop();
		for (std::list<GameObject*>::iterator it = GO->children.begin(); it != GO->children.end(); ++it)
		{
			stackGO.push(*it);
		}
		GO->transform->modelMatrixGlobal = GO->parent->transform->modelMatrixGlobal.Mul(GO->transform->modelMatrixLocal);
	}
}
