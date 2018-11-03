#include "Transform.h"
#include "MathGeoLib.h"

Transform::Transform()
{
}


Transform::~Transform()
{
}

float* Transform::getModelMatrix() const
{
	math::float4x4 tMat = float4x4::identity;
	tMat[0][3] = position.x; tMat[1][3] = position.y; tMat[2][3] = position.z;
	math::float4x4 rMat = math::Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z).ToFloat4x4();
	math::float4x4 sMat = float4x4::identity;
	sMat[0][0] *= scale.x; sMat[1][1] *= scale.y; sMat[2][2] *= scale.z;

	return &(sMat * tMat * rMat)[0][0];
}
