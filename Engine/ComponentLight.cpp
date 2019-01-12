#include "ComponentLight.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"
#include "ModuleScene.h"
#include "ModuleEditorCamera.h"
#include "ModuleSpacePartitioning.h"
#include "AABBTree.h"


ComponentLight::ComponentLight() : Component(ComponentTypes::LIGHT_COMPONENT) 
{
}

void ComponentLight::EditorDraw()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Light"))
	{
		int lType = (int)lightType;
		if (ImGui::Combo("Light type", &lType, "Directional\0Point\0Spot\0")) {
			lightType = (LightTypes)lType;
			CalculateGuizmos();
		}
		ImGui::ColorEdit3("Light Color", &color[0]);
		switch (lightType)
		{
		case LightTypes::SPOT:
			ImGui::Text("%.3f %.3f %.3f", spotDistance, spotEndRadius, cos(outterAngle));
			if (ImGui::InputFloat("Inner angle", &innerAngle, .01f, .1f))
			{
				if (innerAngle > outterAngle)
					innerAngle = outterAngle;
				if (innerAngle < .0f)
					innerAngle = .0f;
				CalculateGuizmos();
			}
			if (ImGui::InputFloat("Outter angle", &outterAngle, .01f, .1f))
			{
				if (outterAngle < innerAngle)
					outterAngle = innerAngle;
				if (outterAngle < .0f)
					innerAngle = .0f;
				CalculateGuizmos();
			}
		case LightTypes::POINT:			
			if (ImGui::InputFloat("Constant attenuation", &attenuation[0], .001f, .1f, "%.15f") ||
				ImGui::InputFloat("Linear attenuation", &attenuation[1], .001f, .1f, "%.15f") ||
				ImGui::InputFloat("Quadric attenuation", &attenuation[2], .001f, .1f, "%.15f") ||
				ImGui::SliderFloat("Influence zone (0.1 ~ Real)", &influence, .1f, 1.f))
			{
				CalculateGuizmos();
			}
			break;
		}
	}
	ImGui::PopID();
}

void ComponentLight::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("lightType"); writer.Int((int)lightType);
	writer.String("color");
	writer.StartArray();
	writer.Double(color.x);
	writer.Double(color.y);
	writer.Double(color.z);
	writer.EndArray();
	writer.String("attenuation");
	writer.StartArray();
	writer.Double(attenuation.x);
	writer.Double(attenuation.y);
	writer.Double(attenuation.z);
	writer.EndArray();
	writer.String("inner");
	writer.Double(innerAngle);
	writer.String("outter");
	writer.Double(outterAngle);
	writer.String("lightInfluence");
	writer.Double(influence);
	writer.EndObject();
}

void ComponentLight::UnSerialize(rapidjson::Value & value)
{
	lightType = (LightTypes)value["lightType"].GetInt();
	color.x = value["color"][0].GetDouble();
	color.y = value["color"][1].GetDouble();
	color.z = value["color"][2].GetDouble();
	attenuation.x = value["attenuation"][0].GetDouble();
	attenuation.y = value["attenuation"][1].GetDouble();
	attenuation.z = value["attenuation"][2].GetDouble();
	innerAngle = value["inner"].GetDouble();
	outterAngle = value["outter"].GetDouble();
	CalculateGuizmos();
}

ComponentLight * ComponentLight::Clone()
{
	ComponentLight* clone = new ComponentLight();
	clone->lightType = lightType;
	clone->color = color;
	clone->attenuation = attenuation;
	clone->innerAngle = innerAngle;
	clone->outterAngle = outterAngle;
	clone->CalculateGuizmos();
	return clone;
}

bool ComponentLight::ConeContainsAABB(const AABB &aabb) const
{
	//TODO:Update lighting aabb tree
	float3 lightPos = owner->transform->getGlobalPosition();
	//check if spot light center passes through the aabb
	LineSegment line;
	line.a = lightPos + owner->transform->front * spotDistance;
	line.b = lightPos;
	if (line.Intersects(aabb))
		return true;
	
	//otherwise check if the cone contains any corner
	for (unsigned i = 0u; i < 8u; ++i)
	{
		float h = aabb.CornerPoint(i).Distance(lightPos);
		float phi = owner->transform->front.AngleBetween(aabb.CornerPoint(i) - lightPos);
		if (phi < outterAngle && h < spotOutterDistance)
			return true;
	}
	
	return false;

}
void ComponentLight::CalculateGuizmos()
{
	float a = influence * attenuation[2];
	float b = influence * attenuation[1];
	float c = (influence * attenuation[0]) - 1.f;
	float p = (-b + sqrt((b * b - 4.f * a * c)) / (2.f * a));

	if (owner != nullptr)
	{
		switch (lightType)
		{
		case LightTypes::POINT:
			pointSphere.pos = owner->transform->getGlobalPosition();
			pointSphere.r = abs(p);
			break;
		case LightTypes::SPOT:
			spotDistance = abs(p); // max distance is equivalent as the point light radius
			spotOutterDistance = abs(spotDistance / sin(spotDistance));
			spotEndRadius = spotDistance * tan(outterAngle);
			pointSphere.pos = owner->transform->getGlobalPosition() + owner->transform->front * spotDistance;
			pointSphere.r = spotEndRadius > spotDistance ? spotEndRadius : spotDistance;
			break;
		case LightTypes::DIRECTIONAL:
			pointSphere.pos = float3::zero;
			if (App->scene->sceneCamera != nullptr)
				pointSphere.r = App->scene->sceneCamera->zFar;
			else
				pointSphere.r = App->camera->editorCamera.zFar;
		}
		if (owner->fakeGameObjectReference != nullptr)
		{
			App->spacePartitioning->aabbTreeLighting.ReleaseNode(owner->fakeGameObjectReference->treeNode);
			owner->fakeGameObjectReference->treeNode = nullptr;
			owner->fakeGameObjectReference->aaBBGlobal->SetNegativeInfinity();
			owner->fakeGameObjectReference->aaBBGlobal->Enclose(pointSphere);
			App->spacePartitioning->aabbTreeLighting.InsertGO(owner->fakeGameObjectReference);
		}
	}
};