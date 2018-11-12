#include "Transform.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "GameObject.h"

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

void Transform::SetModelMatrix(aiMatrix4x4 mMatrix)
{
	modelMatrixLocal[0][0] = mMatrix.a1;
	modelMatrixLocal[0][1] = mMatrix.a2;
	modelMatrixLocal[0][2] = mMatrix.a3;
	modelMatrixLocal[0][3] = mMatrix.a4;
			   
	modelMatrixLocal[1][0] = mMatrix.b1;
	modelMatrixLocal[1][1] = mMatrix.b2;
	modelMatrixLocal[1][2] = mMatrix.b3;
	modelMatrixLocal[1][3] = mMatrix.b4;
			   
	modelMatrixLocal[2][0] = mMatrix.c1;
	modelMatrixLocal[2][1] = mMatrix.c2;
	modelMatrixLocal[2][2] = mMatrix.c3;
	modelMatrixLocal[2][3] = mMatrix.c4;

	modelMatrixLocal[3][0] = mMatrix.d1;
	modelMatrixLocal[3][1] = mMatrix.d2;
	modelMatrixLocal[3][2] = mMatrix.d3;
	modelMatrixLocal[3][3] = mMatrix.d4;

}



void Transform::RecalcModelMatrix()
{

}


void Transform::PropagateTransform(const float4x4& mMatrix) //get parent model Matrix
{

	modelMatrixGlobal = mMatrix.Mul(modelMatrixLocal);// (mMatrix);
}

void Transform::PropagateTransform() 
{	
	for (std::vector<GameObject*>::iterator it = owner->children.begin(); it != owner->children.end(); ++it)
	{
		(*it)->transform->PropagateTransform(modelMatrixGlobal); //Propagate to son
		(*it)->transform->PropagateTransform(); //Propagate from son to below
	}
}
