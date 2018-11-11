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

void SubModuleEditorWorldInspector::drawNode(GameObject* gObj) const
{
	//TODO: Repeated names causes chaos

	assert(gObj != nullptr);

	ImGuiTreeNodeFlags flags;
	if (gObj->children.size() == 0)
		flags = ImGuiTreeNodeFlags_Leaf;
	else
		flags = ImGuiTreeNodeFlags_None;

	if (ImGui::TreeNodeEx(gObj->name.c_str(), flags))
	{
		if (gObj->children.size() > 0)
		{
			for (std::vector<GameObject*>::iterator it = gObj->children.begin(); it != gObj->children.end(); ++it)
				drawNode(*it);
		}
		ImGui::TreePop();
	}
}
