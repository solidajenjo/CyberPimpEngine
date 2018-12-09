#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "Component.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "MaterialImporter.h"
#include <algorithm>
#include "rapidjson-1.1.0/include/rapidjson/writer.h"

#define WIDGET_WIDTHS 128

std::map<std::string, ComponentMaterial*>ComponentMaterial::materialsLoaded;

ComponentMaterial::ComponentMaterial(float r, float g, float b, float a) : Component(ComponentTypes::MATERIAL_COMPONENT)
{
	color = float4(r, g, b, a);
	program = App->program->phongFlat;
	sprintf_s(name, "Material");
	sprintf_s(texturePath, "");
}

ComponentMaterial::~ComponentMaterial()
{

}

void ComponentMaterial::EditorDraw()
{	
	ImGui::PushID(this);
	char header[1024];
	sprintf_s(header, "Material %s", name);
	ImGui::TextWrapped(header);
		
	ImGui::InputText("Name", &name[0], 2000);
	
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
	writer.String("type"); writer.Int((int)type);
	writer.String("name"); writer.String(name);
	writer.String("texturePath"); writer.String(texturePath);
	writer.String("materialPath"); writer.String(materialPath);
	if (strlen(materialPath) > 0)
	{
		MaterialImporter mi;
		mi.Save(materialPath, this);
	}
	writer.String("r"); writer.Double(color.x);
	writer.String("g"); writer.Double(color.y);
	writer.String("b"); writer.Double(color.z);
	writer.String("a"); writer.Double(color.w);
	writer.String("program"); writer.Int(program);
	writer.String("diffuse"); writer.Double(diffuse);
	writer.String("ambient"); writer.Double(ambient);
	writer.String("specular"); writer.Double(specular);

	writer.EndObject();
}

void ComponentMaterial::UnSerialize(rapidjson::Value & value)
{
	sprintf_s(name, value["name"].GetString());
	sprintf_s(texturePath, value["texturePath"].GetString());
	sprintf_s(materialPath, value["materialPath"].GetString());
	color.x = value["r"].GetDouble();
	color.y = value["g"].GetDouble();
	color.z = value["b"].GetDouble();
	color.w = value["a"].GetDouble();
	diffuse = value["diffuse"].GetDouble();
	ambient = value["ambient"].GetDouble();
	specular = value["specular"].GetDouble();
	program = value["program"].GetInt();
	
}

bool ComponentMaterial::Release()
{
	if (clients > 1) //still have clients -> no destroy
	{
		--clients;
		return false;
	}
	else
	{
		materialsLoaded.erase(materialPath); //unload completely
		return true;
	}
}

ComponentMaterial * ComponentMaterial::GetMaterial(const std::string path)
{
	std::map<std::string, ComponentMaterial*>::iterator it = materialsLoaded.find(path);
	if (it == materialsLoaded.end())
	{
		MaterialImporter mi;
		ComponentMaterial* mat = mi.Load(path.c_str());
		if (mat != nullptr)
		{
			materialsLoaded[path] = mat;
			++mat->clients;
			return mat;
		}
		else
			return nullptr;
	}
	++(*it).second->clients;
	return (*it).second;
}

