#include "Transform.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"

float* Transform::GetModelMatrix()
{	
	return &modelMatrixGlobal[0][0]; 
}


Transform::Transform(GameObject* go) : owner(go)
{
}

void Transform::Rotate(float3 rotations)
{
}

void Transform::Translate(float3 translation)
{
}

void Transform::Scale(float3 scalation)
{
}

void Transform::SetPosition(float3 newPosition)
{
	position = newPosition;
}

void Transform::SetRotation(float3 newRotation)
{
	rotation = newRotation;
}

void Transform::SetScale(float3 newScale)
{
	scale = newScale;
}

void Transform::SetTransform(float3 pos, float3 rot, float3 scl)
{
	
}


void Transform::RecalcModelMatrix()
{
	float4x4 t = float4x4::identity;
	t[0][3] = position.x;
	t[1][3] = position.y;
	t[2][3] = position.z;

	float4x4 r = float4x4::identity;
	r = Quat::FromEulerXZY(DegToRad(rotation.x), DegToRad(rotation.z), DegToRad(rotation.y)).ToFloat4x4() * r;
	//IMGUIZMO
	//TODO: Get Rid of recursivity
	float4x4 s = float4x4::identity;

	s[0][0] *= scale.x;
	s[1][1] *= scale.y;
	s[2][2] *= scale.z;

	modelMatrixLocal = t.Mul(s.Mul(r));

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

	ImGui::PushID(1);
	if (ImGui::DragFloat3("Position", &position.x, 0.1f))
	{
		RecalcModelMatrix();
	}
	ImGui::PopID();
	ImGui::PushID(2);
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f))
	{
		RecalcModelMatrix();
	}
	ImGui::PopID();
	ImGui::PushID(3);
	if (ImGui::DragFloat3("Scale", &scale.x, 0.1f))
	{
		RecalcModelMatrix();
	}
	ImGui::PopID();
}


void Transform::PropagateTransform(const float4x4& mMatrix) //get parent model Matrix
{

	modelMatrixGlobal = mMatrix.Mul(modelMatrixLocal);
}

void Transform::PropagateTransform() 
{	
	for (std::list<GameObject*>::iterator it = owner->children.begin(); it != owner->children.end(); ++it)
	{
		(*it)->transform->PropagateTransform(modelMatrixGlobal); //Propagate to son
		(*it)->transform->PropagateTransform(); //Propagate from son to below
	}
}
