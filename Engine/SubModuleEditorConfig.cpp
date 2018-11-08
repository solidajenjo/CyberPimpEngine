#include "SubModuleEditorConfig.h"
#include "Application.h"
#include "ModuleTime.h"
#include "imgui.h"
#include <vector>
#include <numeric>


void SubModuleEditorConfig::Show()
{
	if (enabled)
	{	
		ImGui::Begin(editorModuleName, &enabled);	
		time[f] = App->appTime->realDeltaTime;
		if (time[f] > 0.f)
			fps[f] = 1000.f / time[f];
		deltaTimeAcc += time[f];
		fpsAcc += fps[f];
		if (App->appTime->framesPassed > 100) //wait until the editor gets stable
		{
			if (fps[f] > maxFps)
				maxFps = fps[f];
			if (time[f] > maxDelta)
				maxDelta = time[f];
			if (fps[f] < minFps)
				minFps = fps[f];
			if (time[f] < minDelta)
				minDelta = time[f];
		}
		f++;
		if (f == fps.size())
			f = 0;
		if (ImGui::CollapsingHeader("Time Module"))
		{
			ImGui::PlotLines("", &time[0], time.size(), f, "Delta Time", .0f, 60.0f, ImVec2(ImGui::GetWindowContentRegionMax().x, 60));
			ImGui::PlotLines("", &fps[0], fps.size(), f, "FPS", .0f, 100.0f, ImVec2(ImGui::GetWindowContentRegionMax().x, 60));

			float timeAvg = deltaTimeAcc / App->appTime->framesPassed;
			float fpsAvg = fpsAcc / App->appTime->framesPassed;
			ImGui::Columns(2);
			ImGui::Text("Average Delta Time %.3f", timeAvg);
			ImGui::Text("Average FPS %.3f", fpsAvg);
			ImGui::Text("Real clock %.0f", App->appTime->realClock);
			ImGui::Text("Game clock %.0f", App->appTime->gameClock);
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
		ImGui::End();
	}
}



