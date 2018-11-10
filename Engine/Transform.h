#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"

class GameObject;

class Transform
{
public:
	Transform(GameObject* go);

	void Rotate(float3 rotations);
	void Translate(float3 direction, float amount);
	void Scale(float3 axis, float amount);
	float* GetModelMatrix();
	
private:

	void PropagateModelMatrix(float4x4 modelParent);
	void PropagateModelMatrix();
	void RecalcModelMatrix();
//members
	float3 position = float3(0.f, 0.f, 0.f); 
	float3 rotation = float3(0.f, 0.f, 0.f);
	float3 scale = float3(1.f, 1.f, 1.f);

	float4x4 modelMatrix = float4x4::zero;
	
	GameObject* owner = nullptr;
};

#endif