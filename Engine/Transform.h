#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "Component.h"

class GameObject;

class Transform : public Component
{
	friend class ModuleModelLoader; //recalc the modelMatrix permission

public:
	Transform(GameObject* go);

	void Rotate(const float3& rotations);
	void Translate(const float3& translation);
	void Scale(const float3& scalation);

	void SetPosition(const float3& newPosition); //Sets the local position relative to his parent
	void SetRotation(const float3& newRotation); //Sets the local rotation relative to his parent provided by newRotation in rads
	void SetScale(const float3& newScale); //Sets the local scale relative to his parent
	void SetTransform(const float3& pos, const float3& rot, const float3& scl);

	//TODO: Create getters
	float* GetModelMatrix(); //returns pointer the model matrix[0][0]
	void PropagateTransform();

	void EditorDraw() override;

private:

	void RecalcModelMatrix();

public:
	//members
	Quat rotQuat = Quat::identity;
	float4x4 tMat = float4x4::identity;
	float4x4 sMat = float4x4::identity;
	
	float3 position = float3(0.f, 0.f, 0.f); 
	float3 rotation = float3(0.f, 0.f, 0.f);
	float3 scale = float3(1.f, 1.f, 1.f);

	float3 front = float3(0.f, 0.f, 0.f);
	float3 up = float3(0.f, 0.f, 0.f);
	float3 right = float3(0.f, 0.f, 0.f);

	float4x4 modelMatrixLocal = float4x4::identity;
	float4x4 modelMatrixGlobal = float4x4::identity;
	
	GameObject* owner = nullptr;
};

#endif