#include "Transform.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "GameObject.h"

float4x4 Transform::GetModelMatrix()
{	
	//return modelMatrix; Temporary disabled 
	float4x4 mM = float4x4::identity;
	return  mM;
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

void Transform::SetModelMatrix(aiMatrix4x4 mMatrix)
{
	modelMatrix[0][0] = mMatrix.a1;
	modelMatrix[0][1] = mMatrix.a2;
	modelMatrix[0][2] = mMatrix.a3;
	modelMatrix[0][3] = mMatrix.a4;

	modelMatrix[1][0] = mMatrix.b1;
	modelMatrix[1][1] = mMatrix.b2;
	modelMatrix[1][2] = mMatrix.b3;
	modelMatrix[1][3] = mMatrix.b4;

	modelMatrix[2][0] = mMatrix.c1;
	modelMatrix[2][1] = mMatrix.c2;
	modelMatrix[2][2] = mMatrix.c3;
	modelMatrix[2][3] = mMatrix.c4;

	modelMatrix[3][0] = mMatrix.d1;
	modelMatrix[3][1] = mMatrix.d2;
	modelMatrix[3][2] = mMatrix.d3;
	modelMatrix[3][3] = mMatrix.d4;

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
