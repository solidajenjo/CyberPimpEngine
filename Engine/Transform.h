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
	float4x4 GetModelMatrix(); //returns pointer the model matrix[0][0]
	
private:

	void PropagateTransform(float3 pos, float3 rot, float3 scl);
	void PropagateTransform();
	void RecalcModelMatrix();
//members
	float3 position = float3(0.f, 0.f, 0.f); 
	float3 rotation = float3(0.f, 0.f, 0.f);
	float3 scale = float3(1.f, 1.f, 1.f);

	float4x4 modelMatrix;// = aiMatrix4x4();  // identity matrix
	
	GameObject* owner = nullptr;
};

#endif