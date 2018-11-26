#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "imgui/imgui.h"

#define WIDGET_WIDTHS 128

ComponentMaterial::ComponentMaterial(float r, float g, float b, float a) : Component("Material")
{
	color = float4(r, g, b, a);
	program = App->program->phongFlat;
}

void ComponentMaterial::EditorDraw()
{
	ImGui::PushID(this);
	std::string header = "Material " + name;
	ImGui::TextWrapped(header.c_str());
	
	sprintf_s(nameBuff, 4096, "%s", name.c_str());
	if (ImGui::InputText("Name", &nameBuff[0], 2000))
	{
		name = std::string(nameBuff);
	}
	ImGui::ColorPicker4("Color", &color[0]);
	ImGui::TextWrapped("Texture");
	ImGui::SliderFloat("Ambient", &ambient, 0.f, 1.f);
	ImGui::SliderFloat("Diffuse", &diffuse, 0.f, 1.f);
	ImGui::SliderFloat("Specular", &specular, 0.f, 1.f);

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
