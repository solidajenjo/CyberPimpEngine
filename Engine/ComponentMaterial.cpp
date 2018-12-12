#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentMap.h"
#include "Component.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "MaterialImporter.h"
#include <algorithm>
#include "rapidjson-1.1.0/include/rapidjson/writer.h"
#include "crossguid/include/crossguid/guid.hpp"

#define WIDGET_WIDTHS 128

std::map<std::string, ComponentMaterial*>ComponentMaterial::materialsLoaded;

ComponentMaterial::ComponentMaterial(float r, float g, float b, float a) : Component(ComponentTypes::MATERIAL_COMPONENT)
{
	diffuseColor = float4(r, g, b, a);
	program = App->program->phongFlat;
	
}

ComponentMaterial::~ComponentMaterial()
{
	if (texture != nullptr && texture->Release())
		RELEASE(texture);
	if (normal != nullptr && normal->Release())
		RELEASE(normal);
	if (specular != nullptr && specular->Release())
		RELEASE(specular);
	if (occlusion != nullptr && occlusion->Release())
		RELEASE(occlusion);
	if (emissive != nullptr && emissive->Release())
		RELEASE(emissive);
}

void ComponentMaterial::EditorDraw()
{	
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::PushID(this);
		ImGui::Text("Diffuse");
		ImGui::ColorEdit4("Diffuse Color", &diffuseColor[0]);
		if (texture != nullptr && texture->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)texture->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			ImGui::Button("Drop a diffuse map");
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEX_NUM"))
			{
				assert(payload->DataSize == sizeof(unsigned));
				//texture = *(unsigned*)payload->Data;
			}
		}
		ImGui::Separator();
		ImGui::Text("Specular");
		ImGui::ColorEdit4("Specular Color", &specularColor[0]);
		if (specular != nullptr && specular->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)specular->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			ImGui::Button("Drop a specular map");
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEX_NUM"))
			{
				assert(payload->DataSize == sizeof(unsigned));
				//specular = *(unsigned*)payload->Data;
			}
		}
		ImGui::Separator();
		ImGui::Text("Emissive");
		ImGui::ColorEdit4("Emissive Color", &specularColor[0]);
		if (emissive != nullptr && emissive->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)emissive->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			ImGui::Button("Drop an emissive map");
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEX_NUM"))
			{
				assert(payload->DataSize == sizeof(unsigned));
				//emissive = *(unsigned*)payload->Data;
			}
		}
		ImGui::Separator();
		ImGui::Text("Normal");
		if (normal != nullptr && normal->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)normal->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			ImGui::Button("Drop a normal map");
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEX_NUM"))
			{
				assert(payload->DataSize == sizeof(unsigned));
				//normal = *(unsigned*)payload->Data;
			}
		}
		ImGui::Separator();
		ImGui::SliderFloat("Ambient", &kAmbient, 0.f, 1.f);
		ImGui::SliderFloat("Diffuse", &kDiffuse, 0.f, 1.f);
		ImGui::SliderFloat("Specular", &kSpecular, 0.f, 1.f);
		ImGui::InputFloat("Shininess", &shininess, 0.1f, 0.5f);
		ImGui::Separator();
		ImGui::PopID();
	}
}

void ComponentMaterial::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
	if (strlen(materialPath) == 0)
	{
		xg::Guid guid = xg::newGuid();
		std::string uuid = guid.str();
		std::string matPath = "Library/Materials/" + uuid + ".mat";
		sprintf_s(materialPath, matPath.c_str());
	}
	MaterialImporter mi;
	mi.Save(materialPath, this);
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("materialPath"); writer.String(materialPath);
	writer.EndObject();
		
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

