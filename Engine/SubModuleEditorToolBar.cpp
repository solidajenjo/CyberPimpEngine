#include "SubModuleEditorToolBar.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "imgui.h"
#include <vector>


SubModuleEditorToolBar::SubModuleEditorToolBar(char* editorModuleName) : SubModuleEditor(editorModuleName), toolBarHeight(50)
{	
}


SubModuleEditorToolBar::~SubModuleEditorToolBar()
{
}

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
	ImGui::Text("ToolBar");
	ImGui::End();
}
