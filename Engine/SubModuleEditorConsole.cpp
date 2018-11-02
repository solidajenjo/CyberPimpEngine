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
		ImGui::TextWrapped(App->consoleBuffer->begin());
		ImGui::End();
	}
}
