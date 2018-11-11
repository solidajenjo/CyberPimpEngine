#include "SubModuleEditorConsole.h"
#include "Application.h"
#include "imgui/imgui.h"


void SubModuleEditorConsole::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.c_str(), &enabled);
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0.108f, 0.93f, 0.1f));
		
		ImGui::TextUnformatted(App->consoleBuffer->begin());
		if (lastSize != App->consoleBuffer->size())
		{
			lastSize = App->consoleBuffer->size();
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::PopStyleColor();
		ImGui::End();
	}
}
