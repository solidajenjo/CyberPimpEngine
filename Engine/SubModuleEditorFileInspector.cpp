#include "SubModuleEditorFileInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "GameObject.h"

SubModuleEditorFileInspector::SubModuleEditorFileInspector(const std::string & editorModuleName) : SubModuleEditor(editorModuleName)
{
	
}

void SubModuleEditorFileInspector::Show()
{
	ImGui::Begin("Files");
	App->scene->ShowHierarchy(false);
	ImGui::End();
}

