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
			GameObject* newGO = new GameObject("Empty");
			newGO->transform->PropagateTransform();
			App->scene->InsertGameObject(newGO);
		}
		App->scene->ShowHierarchy();
		ImGui::End();
	}
}


