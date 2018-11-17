#include "SubModuleEditorConfig.h"
#include "Application.h"
#include "ModuleTime.h"
#include "ModuleCamera.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Assimp/include/assimp/version.h"

#include <list>

#define WIDGET_WIDTHS 128

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
			ImGui::PlotLines("", &time[0], time.size(), f, "Real Delta Time", .0f, .1f, ImVec2(ImGui::GetWindowContentRegionMax().x, 60));
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
			ImGui::SliderFloat("Z Near", &App->camera->zNear, 0.01f, 2000.f);
			ImGui::SliderFloat("Z Far", &App->camera->zFar, 0.01f, 2000.f);
			ImGui::SliderFloat("Movement speed", &App->camera->moveSpeed, 0.01f, 50.f);
			ImGui::SliderFloat("Rotation speed", &App->camera->rotSpeed, 0.01f, 50.f);
			ImGui::SliderFloat("Zoom speed", &App->camera->zoomSpeed, 0.01f, 50.f);
		}
		if (ImGui::CollapsingHeader("Textures Module"))
		{
			if (ImGui::Button("Clean"))
			{
				App->textures->CleanUp();
			}
			unsigned id = 0;
			for (std::vector<unsigned>::iterator it = App->textures->textures.begin(); it != App->textures->textures.end(); ++it)
			{
				++id;
				std::string tNum = "Texture " + std::to_string(*it);
				if (ImGui::TreeNode(tNum.c_str()))
				{
					ImGui::Image((void*)(intptr_t)*it, ImVec2(WIDGET_WIDTHS, WIDGET_WIDTHS), ImVec2(0, 1), ImVec2(1, 0));
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						unsigned texNum = *it;
						ImGui::SetDragDropPayload("TEX_NUM", &texNum, sizeof(unsigned));
						ImGui::EndDragDropSource();
					}
					ImGui::PushID(id);
					if (ImGui::TreeNode("MipMaps"))
					{
						int w = 2, h = 2;
						int miplevel = 0;
						for (; w > 1 && h > 1; ++miplevel)
						{
							glBindTexture(GL_TEXTURE_2D, (*it));
							glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
							glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
							if (h != 0 && w != 0)
							{
								std::string s = "MIPMAP " + std::to_string(miplevel) + " " + std::to_string(w) + "x" + std::to_string(h);
								ImGui::TextWrapped(s.c_str());
							}
							glBindTexture(GL_TEXTURE_2D, 0);
						}
						ImGui::TreePop();
					}
					ImGui::PopID();
					ImGui::TreePop();
					ImGui::Separator();
					if (ImGui::Button("Remove"))
					{
						//TODO:Remove texture modify App->textures->textures from vector to list
					}
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
				}
			}
		}
				
		if (ImGui::CollapsingHeader("Scene Module"))
		{
			if (ImGui::Button("Clear scene"))
			{
				App->renderer->CleanUp();
				App->scene->CleanUp();
			}
		}
		if (ImGui::CollapsingHeader("System"))
		{
			if (ImGui::TreeNode("Memory"))
			{
				ImGui::TextWrapped("Memory data placeholder. Out of time :(");//TODO: Put memory statistics
				ImGui::TreePop();
			}
			std::string dependenciesVersions = "Open GL version 3.3\nSDL Version 2.0.9 \nImGui Version " + std::string(ImGui::GetVersion()) + "\nDevIL Version 1.8.0\nGlew Version 2.1.0\nAssimp version "
				+ std::to_string(aiGetVersionMajor()) + "." + std::to_string(aiGetVersionMinor());

			ImGui::TextWrapped(dependenciesVersions.c_str());
		}
		ImGui::End();
	}
}



