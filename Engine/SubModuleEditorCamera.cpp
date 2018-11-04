#include "SubModuleEditorCamera.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "imgui.h"


SubModuleEditorCamera::SubModuleEditorCamera(char* editorModuleName) : SubModuleEditor(editorModuleName)
{	
	
}


SubModuleEditorCamera::~SubModuleEditorCamera()
{

}

void SubModuleEditorCamera::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName, &enabled);

		int boxHeight = 16, boxWidth;
	
		ImGui::Text("Position");
		ImGui::Columns(3);
		ImVec2 pos = ImGui::GetCursorScreenPos();
		boxWidth = ImGui::GetColumnWidth() - 10;
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("X: %.3f", App->camera->camPos.x);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Y: %.3f", App->camera->camPos.y);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Z: %.3f", App->camera->camPos.z);
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();
		
		ImGui::Text("Forward vector");
		ImGui::Columns(3);
		pos = ImGui::GetCursorScreenPos();
		boxWidth = ImGui::GetColumnWidth() - 10;
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("X: %.3f", App->camera->forward.x);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Y: %.3f", App->camera->forward.y);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Z: %.3f", App->camera->forward.z);
		ImGui::Columns(1);
		ImGui::Text("Up vector");
		ImGui::Columns(3);
		pos = ImGui::GetCursorScreenPos();
		boxWidth = ImGui::GetColumnWidth() - 10;
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("X: %.3f", App->camera->up.x);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Y: %.3f", App->camera->up.y);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Z: %.3f", App->camera->up.z);
		ImGui::Columns(1);
		ImGui::Text("Right vector");
		ImGui::Columns(3);
		pos = ImGui::GetCursorScreenPos();
		boxWidth = ImGui::GetColumnWidth() - 10;
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("X: %.3f", App->camera->right.x);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Y: %.3f", App->camera->right.y);
		ImGui::NextColumn();
		pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
		ImGui::Text("Z: %.3f", App->camera->right.z);
		ImGui::Columns(1);

		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();

		ImGui::SliderFloat("FOV", &App->camera->vFov, 10, 180);
		ImGui::InputFloat("Z Near", &App->camera->zNear);
		ImGui::InputFloat("Z Far", &App->camera->zFar);
		ImGui::InputFloat("Movement speed", &App->camera->moveSpeed, 0.01f, 0.5f);
		ImGui::InputFloat("Rotation speed", &App->camera->rotSpeed, 0.01f, 0.5f);
		ImGui::InputFloat("Zoom speed", &App->camera->zoomSpeed, 0.01f, 0.5f);
		ImGui::End();
	}
}
