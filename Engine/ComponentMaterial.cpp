#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "imgui/imgui.h"
#include "rapidjson-1.1.0/include/rapidjson/writer.h"

#define WIDGET_WIDTHS 128

ComponentMaterial::ComponentMaterial(float r, float g, float b, float a) : Component("Material")
{
	color = float4(r, g, b, a);
	program = App->program->phongFlat;
	sprintf_s(nameBuff, "Material");
	sprintf_s(textureName, "");
}

void ComponentMaterial::EditorDraw()
{
	ImGui::PushID(this);
	char header[1024];
	sprintf_s(header, "Material %s", nameBuff);
	ImGui::TextWrapped(header);
		
	ImGui::InputText("Name", &nameBuff[0], 2000);
	
	ImGui::ColorEdit4("Color", &color[0]);
	ImGui::SliderFloat("Ambient", &ambient, 0.f, 1.f);
	ImGui::SliderFloat("Diffuse", &diffuse, 0.f, 1.f);
	ImGui::SliderFloat("Specular", &specular, 0.f, 1.f);
	ImGui::TextWrapped("Texture");
	if (texture != 0)
	{
		ImGui::Image((void*)(intptr_t)texture, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
	}
	else
	{
		ImGui::Button("Drop a texture");
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEX_NUM"))
		{
			assert(payload->DataSize == sizeof(unsigned));
			texture = *(unsigned*)payload->Data;
		}
	}
	ImGui::Separator();
	ImGui::PopID();
}

void ComponentMaterial::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
	writer.StartObject();

	writer.String("name"); writer.String(nameBuff);
	writer.String("texture"); writer.String(textureName);
	writer.String("r"); writer.Double(color.x);
	writer.String("g"); writer.Double(color.y);
	writer.String("b"); writer.Double(color.z);
	writer.String("a"); writer.Double(color.w);

	writer.String("diffuse"); writer.Double(diffuse);
	writer.String("ambient"); writer.Double(ambient);
	writer.String("specular"); writer.Double(specular);

	writer.EndObject();
}
