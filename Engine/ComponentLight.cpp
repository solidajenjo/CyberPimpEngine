#include "ComponentLight.h"
#include "Globals.h"
#include "Application.h"
#include "imgui/imgui.h"

ComponentLight::ComponentLight() : Component(ComponentTypes::LIGHT_COMPONENT)
{
	CalculateSphereRadius();
}

void ComponentLight::EditorDraw()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Light"))
	{
		int lType = (int)lightType;
		if (ImGui::Combo("Light type", &lType, "Directional\0Point\0Spot\0")) {
			lightType = (LightTypes)lType;
		}
		ImGui::ColorEdit3("Light Color", &color[0]);
		switch (lightType)
		{
		case LightTypes::SPOT:
			if (ImGui::InputFloat("Inner angle", &innerAngle, .01f, .1f))
			{
				if (innerAngle > outterAngle)
					innerAngle = outterAngle;

			}
			if (ImGui::InputFloat("Outter angle", &outterAngle, .01f, .1f))
			{
				if (outterAngle < innerAngle)
					outterAngle = innerAngle;
			}
		case LightTypes::POINT:			
			if (ImGui::InputFloat("Constant attenuation", &attenuation[0], .001f, .1f, "%.15f") ||
				ImGui::InputFloat("Linear attenuation", &attenuation[1], .001f, .1f, "%.15f") ||
				ImGui::InputFloat("Quadric attenuation", &attenuation[2], .001f, .1f, "%.15f") ||
				ImGui::SliderFloat("Influence zone (0.1 ~ Real)", &influence, .1f, 1.f))
			{
				CalculateSphereRadius();
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
	CalculateSphereRadius();
}

ComponentLight * ComponentLight::Clone()
{
	ComponentLight* clone = new ComponentLight();
	clone->lightType = lightType;
	clone->color = color;
	clone->attenuation = attenuation;
	clone->innerAngle = innerAngle;
	clone->outterAngle = outterAngle;
	return clone;
}
