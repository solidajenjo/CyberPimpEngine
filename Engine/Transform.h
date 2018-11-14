#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "Assimp/include/assimp/types.h"

class GameObject;

class Transform
{
public:
	Transform(GameObject* go);

	void Rotate(float3 rotations);
	void Translate(float3 translation);
	void Scale(float3 scalation);

	void SetPosition(float3 newPosition);
	void SetRotation(float3 newRotation);
	void SetScale(float3 newScale);
	void SetTransform(float3 pos, float3 rot, float3 scl);
	void SetModelMatrix(aiMatrix4x4 mMatrix);

	//TODO: Create getters
	float* GetModelMatrix(); //returns pointer the model matrix[0][0]
	void PropagateTransform();
	void RecalcModelMatrix();

private:

	void PropagateTransform(const float4x4 &mMatrix);
		
//members

public: //TODO: Make private when the getters are done

	float3 position = float3(0.f, 0.f, 0.f); 
	float3 rotation = float3(0.f, 0.f, 0.f);
	float3 scale = float3(1.f, 1.f, 1.f);

	float4x4 modelMatrixLocal = float4x4::identity;
	float4x4 modelMatrixGlobal = float4x4::identity;
	
	GameObject* owner = nullptr;
};

#endif