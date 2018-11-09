#include "SubModuleEditorWorldInspector.h"
#include "Application.h"
#include "imgui/imgui.h"


void SubModuleEditorWorldInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		
		ImGui::End();
	}
}
