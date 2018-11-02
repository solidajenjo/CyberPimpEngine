#include "SubModuleEditorWorldInspector.h"
#include "Application.h"
#include "imgui.h"


SubModuleEditorWorldInspector::SubModuleEditorWorldInspector(char* editorModuleName) : SubModuleEditor(editorModuleName)
{	

}


SubModuleEditorWorldInspector::~SubModuleEditorWorldInspector()
{
}

void SubModuleEditorWorldInspector::Show()
{
	if (enabled)
	{
		ImGui::Begin(editorModuleName, &enabled);
		if (ImGui::TreeNode("World"))
		{
			if (ImGui::TreeNode("BakerHouse"))
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}
}
