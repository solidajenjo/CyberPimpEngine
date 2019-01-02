#include "ComponentLight.h"
#include "imgui/imgui.h"


ComponentLight::~ComponentLight()
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
		}
		ImGui::ColorEdit4("Diffuse Color", &color[0]);
		switch (lightType)
		{
		case LightTypes::POINT:
			ImGui::InputFloat("Constant attenuation", &attenuation[0]);
			ImGui::InputFloat("Linear attenuation", &attenuation[1]);
			ImGui::InputFloat("Quadric attenuation", &attenuation[2]);
			break;
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
}

ComponentLight * ComponentLight::Clone()
{
	return nullptr;
}
