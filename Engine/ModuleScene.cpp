#include "ModuleScene.h"
#include "GameObject.h"
#include "imgui/imgui.h"

bool ModuleScene::Init()
{
	LOG("Init Scene module");
	return true;
}

update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Cleaning scene GameObjects.");
	for (std::vector<GameObject*>::reverse_iterator it = sceneGameObjects.rbegin(); it != sceneGameObjects.rend(); ++it) //must be cleaned backwards because dependencies
	{
		RELEASE(*it);
	}
	sceneGameObjects.resize(0);
	LOG("Cleaning scene GameObjects. Done");
	selected = nullptr;
	return true;
}

void ModuleScene::insertGameObject(GameObject * newGO)
{
	sceneGameObjectsHierarchy.push_back(newGO);
	flattenHierarchy(newGO);
}

void ModuleScene::destroyGameObject(GameObject * newGO)
{
}

void ModuleScene::showHierarchy()
{

	for (std::vector<GameObject*>::const_iterator it = sceneGameObjectsHierarchy.begin(); it != sceneGameObjectsHierarchy.end(); ++it)
	{
		drawNode(*it);
	}
}

void ModuleScene::drawNode(GameObject* gObj)
{
	assert(gObj != nullptr);

	ImGuiTreeNodeFlags flags;
	if (gObj->children.size() == 0)
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
	else
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	ImGui::PushID(&gObj);
	if (gObj->selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	bool node_open = ImGui::TreeNodeEx(gObj->name.c_str(), flags);
	if (ImGui::IsItemClicked())
	{
		gObj->selected = !gObj->selected;
		if (gObj->selected && selected != gObj)
		{
			selected != nullptr ? selected->selected = false : 1;
			selected = gObj;
		}
		else if (!gObj->selected && selected == gObj)
		{
			selected = nullptr;
		}
	}
	if (node_open)
	{
		if (gObj->children.size() > 0)
		{
			for (std::list<GameObject*>::iterator it = gObj->children.begin(); it != gObj->children.end(); ++it)
				drawNode(*it);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();

}

const std::vector<GameObject*>* ModuleScene::getSceneGameObjects() const
{
	return &sceneGameObjects;
}

void ModuleScene::flattenHierarchy(GameObject* go)
{
	sceneGameObjects.push_back(go);
	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
		flattenHierarchy(*it);

}
