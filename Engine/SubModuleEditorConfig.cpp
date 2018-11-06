#include "SubModuleEditorConfig.h"
#include "Application.h"
#include "imgui.h"
#include "SDL.h"


void SubModuleEditorConfig::Show()
{
	if (enabled)
	{		
		if (SDL_GetTicks() >= nextSecond)
		{
			fpsList.emplace_back(frameCounter * 5);
			fpsList.pop_front();
			nextSecond = SDL_GetTicks() + 200;
			frameCounter = 0;
		}
		ImGui::Begin("Configuration", &enabled);
		lineListPlotter(fpsList);
		++frameCounter;
		ImGui::Text("FPS updated every 200 ms");
		ImGui::End();
	}
}

void SubModuleEditorConfig::lineListPlotter(const std::list<float>& valList)
{
	unsigned cont = 0; 
	int w = 1;
	ImVec2 pos = ImVec2(ImGui::GetWindowPos().x + 10, ImGui::GetWindowPos().y + 27);
	unsigned bottom = pos.y + 500;
	std::list<float>::const_iterator it = valList.cbegin();
	pos.y = bottom - *it * 4;
	//LOG("%d", bottom);
	for (; it != valList.end(); ++it)
	{		
		if (cont++ < valList.size() - 1)
		{
			unsigned nextX = cont * 4;
			unsigned nextY = bottom - *it * 4;
			ImGui::GetWindowDrawList()->AddLine(pos, ImVec2(nextX, nextY), IM_COL32(100, 100, 100, 255));
			pos.x = nextX;
			pos.y = nextY;
		}
		else if (it == valList.cbegin())
		{
			pos.y = bottom - *it;
		}
	}
	pos = ImGui::GetCursorScreenPos();
	//LOG("END");
}
