#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "Component.h"

class GameObject;
class ModuleScene;

class Transform : public Component
{

public:
	Transform(GameObject* go);
	Transform() : Component(ComponentTypes::TRANSFORM_COMPONENT) {};

	void Rotate(const float3& rotations); //in local coords
	void Translate(const float3& translation); //in parent's coords
	void Scale(const float3& scalation); //in local coords

	void SetPosition(const float3& newPosition); //Sets the local position relative to his parent
	void SetRotation(const float3& newRotation); //Sets the local rotation relative to his parent provided by newRotation in rads
	void SetScale(const float3& newScale); //Sets the local scale relative to his parent
	void SetTransform(const float3& pos, const float3& rot, const float3& scl);

	float* GetModelMatrix(); //returns pointer the model matrix[0][0]
	float3 getGlobalPosition();
	void UpdateAABB() const;
	void PropagateTransform();
	void NewAttachment();
	void EditorDraw() override;

	Transform* Clone() override;

	void ExtractLocalTransformFromMatrix();
	void RecalcModelMatrix();

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	void UnSerialize(rapidjson::Value &value) override;

	//members
	Quat rotQuat = Quat::identity;
	
	float3 position = float3(0.f, 0.f, 0.f); 
	float3 localPosition = float3(0.f, 0.f, 0.f); 
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