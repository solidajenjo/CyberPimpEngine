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
		App->scene->showHierarchy();
		ImGui::End();
	}
}


