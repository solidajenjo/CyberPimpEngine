#include "SubModuleEditorWorldInspector.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ModuleScene.h"
#include "GameObject.h"


void SubModuleEditorWorldInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		
		for (std::vector<GameObject*>::iterator it = App->scene->sceneGameObjects.begin(); it != App->scene->sceneGameObjects.end(); ++it)
		{
			drawNode(*it);
		}
		ImGui::End();
	}
}

void SubModuleEditorWorldInspector::drawNode(GameObject* gObj)
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
			for (std::vector<GameObject*>::iterator it = gObj->children.begin(); it != gObj->children.end(); ++it)
				drawNode(*it);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();

}
