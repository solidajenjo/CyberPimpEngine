#include "SubModuleEditorConsole.h"
#include "Application.h"
#include "imgui/imgui.h"


void SubModuleEditorConsole::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.c_str(), &enabled);
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0.108f, 0.93f, 0.1f));
		ImGui::TextWrapped(App->consoleBuffer->begin());
		ImGui::PopStyleColor();
		ImGui::End();
	}
}
