#include "SubModuleEditorConfig.h"
#include "Application.h"
#include "ModuleTime.h"
#include "ModuleCamera.h"
#include "imgui/imgui.h"
#include <vector>


void SubModuleEditorConfig::Show()
{
	if (enabled)
	{					
		ImGui::Begin(editorModuleName.c_str(), &enabled);	
		frames++;
		timer -= App->appTime->realDeltaTime;
		float timeAvg = App->appTime->realClock / App->appTime->framesPassed;
		if (timer <= 0.f) 
		{
			timer = SUB_SAMPLE_TIME;
			fps[f] = frames * SUB_SAMPLE_MULTIPLIER;
			fpsAcc += fps[f];			
			fpsAvg = fpsAcc / fpsAvgDivider;
			fpsAvgDivider += 1.f;
			time[f] = App->appTime->realDeltaTime;
			if (fpsAvg > maxFps)
				maxFps = fpsAvg;
			if (timeAvg > maxDelta)
				maxDelta = timeAvg;
			if (fpsAvg < minFps)
				minFps = fpsAvg;
			if (timeAvg < minDelta)
				minDelta = timeAvg;
			frames = 0;
			f++;
			if (f == fps.size())
				f = 0;
		}
		if (ImGui::CollapsingHeader("Time Module"))
		{
			ImGui::PlotLines("", &time[0], time.size(), f, "Real Delta Time", .0f, 60.0f, ImVec2(ImGui::GetWindowContentRegionMax().x, 60));
			ImGui::PlotLines("", &fps[0], fps.size(), f, "FPS", .0f, 100.0f, ImVec2(ImGui::GetWindowContentRegionMax().x, 60));
			
			ImGui::Columns(2);
			ImGui::Text("Average Delta Time %.3f", timeAvg);
			ImGui::Text("Average FPS %.3f", fpsAvg);
			ImGui::Text("Real clock seconds %.0f", App->appTime->realClock);
			ImGui::Text("Game clock seconds %.0f", App->appTime->gameClock);
			ImGui::PushItemWidth(ImGui::GetColumnWidth() * .95f);
			ImGui::InputFloat("", &App->appTime->gameClockMultiplier, 0.1f, 10.f);
			ImGui::PopItemWidth();
			ImGui::NextColumn();
			ImGui::Text("Min Delta Time %.3f", minDelta);
			ImGui::Text("Max Delta Time %.3f", maxDelta);
			ImGui::Text("Min FPS        %.3f", minFps);
			ImGui::Text("Max FPS        %.3f", maxFps);
			ImGui::Text("Game time scale");
			ImGui::Columns(1);
		}
		if (ImGui::CollapsingHeader("Camera Module"))
		{
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
			ImGui::Text("X: %.3f", App->camera->frustum.front.x);
			ImGui::NextColumn();
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("Y: %.3f", App->camera->frustum.front.y);
			ImGui::NextColumn();
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("Z: %.3f", App->camera->frustum.front.z);
			ImGui::Columns(1);
			ImGui::Text("Up vector");
			ImGui::Columns(3);
			pos = ImGui::GetCursorScreenPos();
			boxWidth = ImGui::GetColumnWidth() - 10;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("X: %.3f", App->camera->frustum.up.x);
			ImGui::NextColumn();
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("Y: %.3f", App->camera->frustum.up.y);
			ImGui::NextColumn();
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("Z: %.3f", App->camera->frustum.up.z);
			ImGui::Columns(1);
			ImGui::Text("Right vector");
			ImGui::Columns(3);
			pos = ImGui::GetCursorScreenPos();
			boxWidth = ImGui::GetColumnWidth() - 10;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("X: %.3f", App->camera->frustum.WorldRight().x);
			ImGui::NextColumn();
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("Y: %.3f", App->camera->frustum.WorldRight().y);
			ImGui::NextColumn();
			pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x - 2, pos.y), ImVec2(pos.x + boxWidth, pos.y + boxHeight), IM_COL32(100, 100, 100, 255));
			ImGui::Text("Z: %.3f", App->camera->frustum.WorldRight().z);
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
		}
		ImGui::End();
	}
}



