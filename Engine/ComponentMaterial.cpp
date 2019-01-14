#include "ComponentMaterial.h"
#include "ComponentMap.h"
#include "Component.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "ModuleTextures.h"
#include "imgui/imgui.h"
#include "MaterialImporter.h"
#include "GameObject.h"
#include "crossguid/include/crossguid/guid.hpp"
#include "rapidjson-1.1.0/include/rapidjson/writer.h"
#include <stack>

#define WIDGET_WIDTHS 150

std::map<std::string, ComponentMaterial*>ComponentMaterial::materialsLoaded;

ComponentMaterial::ComponentMaterial(float r, float g, float b, float a) : Component(ComponentTypes::MATERIAL_COMPONENT)
{
	diffuseColor = float4(r, g, b, a);
	program = App->program->forwardRenderingProgram;

	texture = new ComponentMap();
	texture->mapId = App->textures->whiteFallback;
	sprintf_s(texture->mapPath, "W_FBACK");

	emissive = new ComponentMap();
	emissive->mapId = App->textures->whiteFallback;
	sprintf_s(emissive->mapPath, "W_FBACK");

	specular = new ComponentMap();
	specular->mapId = App->textures->whiteFallback;
	sprintf_s(specular->mapPath, "W_FBACK");

	occlusion = new ComponentMap();
	occlusion->mapId = App->textures->whiteFallback;
	sprintf_s(occlusion->mapPath, "W_FBACK");

	normal = new ComponentMap();
	normal->mapId = App->textures->blackFallback;
	sprintf_s(normal->mapPath, "W_FBACK");
	
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
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader((std::string("Material - ") + std::string(this->owner->name)).c_str()))
	{		
		if (mapSelectorReference != nullptr)
		{
			MapSelector();
		}
		ImGui::Text("Diffuse");
		ImGui::ColorEdit4("Diffuse Color", &diffuseColor[0]);
		if (texture != nullptr && texture->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)texture->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::Button("Change Diffuse Map", ImVec2(WIDGET_WIDTHS, 20)))
		{
			mapSelectorReference = texture;
			mapSelectorSelected = new ComponentMap();
			mapSelectorSelected->mapId = texture->mapId;
			sprintf_s(mapSelectorSelected->mapPath, texture->mapPath);
		}
		ImGui::Checkbox("Uses alpha", &useAlpha);
		ImGui::Separator();
		ImGui::Text("Specular");
		ImGui::ColorEdit4("Specular Color", &specularColor[0]);
		if (specular != nullptr && specular->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)specular->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::Button("Change Specular Map", ImVec2(WIDGET_WIDTHS, 20)))
		{
			mapSelectorReference = specular;
			mapSelectorSelected = new ComponentMap();
			mapSelectorSelected->mapId = specular->mapId;
			sprintf_s(mapSelectorSelected->mapPath, specular->mapPath);
		}
		ImGui::Separator();
		ImGui::Text("Emissive");
		ImGui::ColorEdit4("Emissive Color", &emissiveColor[0]);
		if (emissive != nullptr && emissive->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)emissive->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::Button("Change Emissive Map", ImVec2(WIDGET_WIDTHS, 20)))
		{
			mapSelectorReference = emissive;
			mapSelectorSelected = new ComponentMap();
			mapSelectorSelected->mapId = emissive->mapId;
			sprintf_s(mapSelectorSelected->mapPath, emissive->mapPath);
		}
		ImGui::Separator();
		ImGui::Text("Normal");
		if (normal != nullptr && normal->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)normal->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::Button("Change Normal Map", ImVec2(WIDGET_WIDTHS, 20)))
		{
			mapSelectorReference = normal;
			mapSelectorSelected = new ComponentMap();
			mapSelectorSelected->mapId = normal->mapId;
			sprintf_s(mapSelectorSelected->mapPath, normal->mapPath);
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("Occlusion");
		if (occlusion != nullptr && occlusion->mapId != 0)
		{
			ImGui::Image((void*)(intptr_t)occlusion->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::Button("Change Occlusion Map", ImVec2(WIDGET_WIDTHS, 20)))
		{
			mapSelectorReference = occlusion;
			mapSelectorSelected = new ComponentMap();
			mapSelectorSelected->mapId = occlusion->mapId;
			sprintf_s(mapSelectorSelected->mapPath, occlusion->mapPath);
		}
		ImGui::Separator();
		ImGui::SliderFloat("Ambient", &kAmbient, 0.f, 1.f);
		ImGui::SliderFloat("Diffuse", &kDiffuse, 0.f, 1.f);
		ImGui::SliderFloat("Specular", &kSpecular, 0.f, 1.f);
		ImGui::InputFloat("Shininess", &shininess, 0.1f, 0.5f);
		ImGui::Separator();		
	}
	ImGui::PopID();
}

void ComponentMaterial::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
	Save();	
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("materialPath"); writer.String(materialPath);
	writer.String("useAlpha"); writer.Bool(useAlpha);
	writer.EndObject();
		
}

void ComponentMaterial::Save()
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

ComponentMaterial * ComponentMaterial::Clone()
{
	ComponentMaterial* newMat = new ComponentMaterial(diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w);
	if (texture != nullptr)
		newMat->texture = ComponentMap::GetMap(texture->mapPath);
	if (normal != nullptr)
	    newMat->normal = ComponentMap::GetMap(normal->mapPath);
	if (specular != nullptr)
		newMat->specular = ComponentMap::GetMap(specular->mapPath);
	if (occlusion != nullptr)
		newMat->occlusion = ComponentMap::GetMap(occlusion->mapPath);
	if (emissive != nullptr)
		newMat->emissive = ComponentMap::GetMap(emissive->mapPath);

	newMat->program = program;
	newMat->specularColor = specularColor;
	newMat->emissiveColor = emissiveColor;
	newMat->kDiffuse = kDiffuse;
	newMat->kAmbient = kAmbient;
	newMat->kSpecular = kSpecular;
	newMat->shininess = shininess;

	sprintf_s(newMat->diffuseMap, diffuseMap);
	sprintf_s(newMat->emissiveMap, emissiveMap);
	sprintf_s(newMat->specularMap, specularMap);
	sprintf_s(newMat->occlusionMap, occlusionMap);
	sprintf_s(newMat->normalMap, normalMap);
	
	sprintf_s(newMat->materialPath, "");
	newMat->Save();
	return newMat;
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

void ComponentMaterial::MapSelector()
{
	ImGui::OpenPopup("Map selector");
	if (ImGui::BeginPopupModal("Map selector"))
	{
		ImGui::BeginChild("Control", ImVec2(0, WIDGET_WIDTHS * 1.2f), true);
		ImGui::Text("Selected");
		ImGui::SameLine();
		ImGui::Image((void*)(intptr_t)mapSelectorSelected->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		if (ImGui::Button("Accept", ImVec2(100, 20)))
		{			
			mapSelectorReference->mapId = mapSelectorSelected->mapId;
			sprintf_s(mapSelectorReference->mapPath, mapSelectorSelected->mapPath);
			mapSelectorReference = nullptr;
			RELEASE(mapSelectorSelected);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 20)))
		{
			mapSelectorReference = nullptr;
			RELEASE(mapSelectorSelected);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndChild();
		ImGui::BeginChild("Maps", ImVec2(0, 0), true);
		GameObject* mapHolder = App->scene->mapFolder;
		std::stack<GameObject*> S;
		S.push(mapHolder);
		ImGui::Columns(2);
		
		ImGui::Text("White Fallback");				
		ImGui::Image((void*)(intptr_t)App->textures->whiteFallback, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::IsItemClicked()) 
		{
			mapSelectorSelected->mapId = App->textures->whiteFallback;
			sprintf_s(mapSelectorSelected->mapPath, "W_FBACK");
		}
		
		ImGui::NextColumn();
		
		ImGui::Text("Black Fallback");
		ImGui::Image((void*)(intptr_t)App->textures->blackFallback, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::IsItemClicked())
		{
			mapSelectorSelected->mapId = App->textures->blackFallback;
			sprintf_s(mapSelectorSelected->mapPath, "B_FBACK");
		}

		while (!S.empty())
		{
			GameObject* go = S.top();
			S.pop();
			if (go != mapHolder)
			{
				ImGui::NextColumn();
				ImGui::Text(go->name);
				assert(!go->components.empty());
				ComponentMap* map = (ComponentMap*)go->components.front();
				ImGui::Image((void*)(intptr_t)map->mapId, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
				if (ImGui::IsItemClicked())
				{
					mapSelectorSelected->mapId = map->mapId;
					sprintf_s(mapSelectorSelected->mapPath, map->mapPath);
				}
			}
			for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
			{
				S.push(*it);
			}
		}
		ImGui::Columns(1);
		ImGui::EndChild();
		ImGui::EndPopup();
	}
}

