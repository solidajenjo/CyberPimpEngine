#include "Transform.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "GameObject.h"

float* Transform::GetModelMatrix()
{
	return &modelMatrix[0][0];
}


Transform::Transform(GameObject * go) : owner(go)
{
	RecalcModelMatrix();
}

void Transform::Rotate(float3 rotations)
{
}

void Transform::Translate(float3 direction, float amount)
{
}

void Transform::Scale(float3 axis, float amount)
{
}

void Transform::RecalcModelMatrix()
{
	math::float4x4 tMat = float4x4::identity;
	tMat[0][3] = position.x; tMat[1][3] = position.y; tMat[2][3] = position.z;
	math::float4x4 rMat = math::Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z)).ToFloat4x4();
	math::float4x4 sMat = float4x4::identity;
	sMat[0][0] *= scale.x; sMat[1][1] *= scale.y; sMat[2][2] *= scale.z;
	modelMatrix = rMat * sMat * tMat;
	PropagateModelMatrix();
}


void Transform::PropagateModelMatrix(float4x4 modelParent)
{	
	modelMatrix = modelParent + modelMatrix;
	for (std::vector<GameObject*>::const_iterator it = owner->children.begin(); it != owner->children.end(); ++it) //propagate transform
	{
		(*it)->transform->PropagateModelMatrix(modelMatrix);
	}
}

void Transform::PropagateModelMatrix()
{	
	for (std::vector<GameObject*>::const_iterator it = owner->children.begin(); it != owner->children.end(); ++it) //propagate transform
	{
		(*it)->transform->PropagateModelMatrix(modelMatrix);
	}
}
