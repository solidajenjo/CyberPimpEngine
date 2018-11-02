#include "SubModuleEditorConsole.h"
#include "Application.h"
#include "imgui.h"


SubModuleEditorConsole::SubModuleEditorConsole(char* editorModuleName) : SubModuleEditor(editorModuleName)
{	

}


SubModuleEditorConsole::~SubModuleEditorConsole()
{
}

void SubModuleEditorConsole::Show() 
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName, &enabled);
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0.108f, 0.93f, 0.1f));
		ImGui::TextWrapped(App->consoleBuffer->begin());
		ImGui::PopStyleColor();
		ImGui::End();
	}
}
