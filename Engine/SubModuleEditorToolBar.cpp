#include "SubModuleEditorToolBar.h"
#include "ModuleEditor.h"
#include "SubModuleEditorGameViewPort.h"
#include "Application.h"
#include "imgui/imgui.h"

void SubModuleEditorToolBar::Show()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolBarHeight));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("ToolBar", nullptr, window_flags);
	{
		const char* items[] = { "1", "10", "100", "1000" };
		static const char* item_current = items[0];
		ImGui::PushItemWidth(100.f);
		if (ImGui::BeginCombo("Scale", item_current))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (item_current == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current = items[n];
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			App->appScale = (float)atoi(item_current);
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	{
		const char* items[] = { "None", "SSAAx2"};
		static const char* item_current = items[0];
		ImGui::PushItemWidth(100.f);
		if (ImGui::BeginCombo("AA", item_current))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (item_current == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current = items[n];
					App->editor->gameViewPort->framebufferDirty = true;
					App->editor->gameViewPort->antialiasing = (SubModuleEditorGameViewPort::AntiaAliasing)n;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}
	ImGui::End();
}
