#include "ComponentMap.h"
#include "imgui/imgui.h"
#include "Application.h"
#include "ModuleTextures.h"

std::map<std::string, ComponentMap*>ComponentMap::mapsLoaded;

void ComponentMap::EditorDraw()
{
	if (ImGui::Checkbox("Use Mip Maps", &useMipMaps))
	{
		LOG("Unloading texture %s", mapPath);
		App->textures->UnLoad(mapId);
		mapId = App->textures->Load(mapPath, useMipMaps);
	}
	if (mapId != 0)
	{
		float width = ImGui::GetWindowContentRegionWidth();
		ImGui::Image((void*)(intptr_t)mapId, ImVec2(width, width), ImVec2(0, 1), ImVec2(1, 0));
	}
}

void ComponentMap::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();
	writer.String("type"); writer.Int((int)type);
	writer.String("mapPath");
	writer.String(mapPath);
	writer.String("useMipMaps");
	writer.Bool(useMipMaps);
	writer.EndObject();
}

bool ComponentMap::Release()
{
	if (clients > 1) //still have clients -> no destroy
	{
		--clients;
		return false;
	}
	else
	{
		mapsLoaded.erase(mapPath); //unload completely
		return true;
	}
}

ComponentMap * ComponentMap::Clone()
{
	if (strlen(mapPath) > 0)
		return GetMap(mapPath);
	else
		return new ComponentMap();
}

ComponentMap* ComponentMap::GetMap(const std::string path, bool useMipMaps)
{
	std::map<std::string, ComponentMap*>::iterator it = mapsLoaded.find(path);
	if (it == mapsLoaded.end())
	{
		ComponentMap* newMap = new ComponentMap();
		newMap->mapId = App->textures->Load(path, useMipMaps);
		sprintf_s(newMap->mapPath, path.c_str());
		++newMap->clients;
		mapsLoaded[newMap->mapPath] = newMap;
		return newMap;		
	}
	else // return copy
	{
		ComponentMap* newMap = new ComponentMap();
		newMap->mapId = (*it).second->mapId;
		sprintf_s(newMap->mapPath, (*it).second->mapPath);	
		return newMap;
	}
}
