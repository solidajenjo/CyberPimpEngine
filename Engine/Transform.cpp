#include "Transform.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "GameObject.h"

float* Transform::GetModelMatrix()
{	
	return &modelMatrix.a1;
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

}

void Transform::SetRotation(float3 newRotation)
{
	
}

void Transform::SetScale(float3 newScale)
{
	
}

void Transform::SetTransform(float3 pos, float3 rot, float3 scl)
{
	
}

void Transform::SetModelMatrix(aiMatrix4x4 mMatrix)
{
	modelMatrix = mMatrix;
}



void Transform::RecalcModelMatrix()
{

}


void Transform::PropagateTransform(float3 pos, float3 rot, float3 scl) //get parent transform
{	
}

void Transform::PropagateTransform()
{	
	
}
