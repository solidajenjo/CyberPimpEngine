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
		if(ImGui::Button("New"))
		{
			GameObject* newGO = new GameObject();
			newGO->name = "EMPTY";
			newGO->transform->PropagateTransform();
			App->scene->insertGameObject(newGO);
		}
		App->scene->showHierarchy();
		ImGui::End();
	}
}


