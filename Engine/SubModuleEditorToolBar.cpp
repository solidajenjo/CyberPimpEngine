#include "SubModuleEditorToolBar.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
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
	
	ImGui::PushItemWidth(100.f);
	if (ImGui::BeginCombo("Scale", scale_item_current))
	{
		for (int n = 0; n < IM_ARRAYSIZE(scale_items); n++)
		{
			bool is_selected = (scale_item_current == scale_items[n]);
			if (ImGui::Selectable(scale_items[n], is_selected))
			{
				scale_item_current = scale_items[n];
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		App->appScale = (float)atoi(scale_item_current);
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	
	ImGui::SameLine();

	ImGui::PushItemWidth(100.f);
	if (ImGui::BeginCombo("AA", aa_item_current))
	{
		for (int n = 0; n < IM_ARRAYSIZE(aa_items); n++)
		{
			bool is_selected = (aa_item_current == aa_items[n]);
			if (ImGui::Selectable(aa_items[n], is_selected))
			{
				aa_item_current = aa_items[n];
				App->editor->gameViewPort->framebufferDirty = true;
				App->editor->gameViewPort->antialiasing = (SubModuleEditorGameViewPort::AntiaAliasing)n;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (App->scene->isPlaying)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.1f, .1f, .1f, 1.f));
	bool b = false;
	if (ImGui::Button("Play", ImVec2(100, 20)) && !App->scene->isPlaying)
	{
		App->scene->SaveScene("/autosave.scn");
		App->scene->isPlaying = true;
		b = true;
	}
	if (App->scene->isPlaying && !b)
		ImGui::PopStyleColor();
	ImGui::SameLine();
	if (ImGui::Button("Pause", ImVec2(100, 20)))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", ImVec2(100, 20)))
	{
		App->scene->LoadScene("User Data/autosave.scn");
		App->scene->isPlaying = false;
	}
	ImGui::End();
}
