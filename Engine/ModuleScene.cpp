#include "ModuleScene.h"
#include "GameObject.h"

#include "imgui/imgui.h"

bool ModuleScene::Init()
{
	LOG("Init Scene module");
	root = new GameObject();
	root->name = "Scene";
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
	root->children.clear();
	LOG("Cleaning scene GameObjects. Done");
	selected = nullptr;
	return true;
}

void ModuleScene::insertGameObject(GameObject * newGO)
{
	assert(newGO != nullptr);
	root->children.push_back(newGO);
	flattenHierarchy(newGO);
}

void ModuleScene::destroyGameObject(GameObject * destroyableGO)
{
	assert(destroyableGO != nullptr);
}

void ModuleScene::showHierarchy()
{

	for (std::list<GameObject*>::const_iterator it = root->children.begin(); it != root->children.end(); ++it)
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
	ImGui::PushID(&gObj->name);
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
