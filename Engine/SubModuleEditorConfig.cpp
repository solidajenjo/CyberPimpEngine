#include "SubModuleEditorConfig.h"
#include "Application.h"
#include "ModuleTime.h"
#include "ModuleEditorCamera.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "ModuleProgram.h"
#include "ModuleSpacePartitioning.h"
#include "QuadTree.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Assimp/include/assimp/version.h"
#include "SDL/include/SDL_video.h"

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
			if (ImGui::Checkbox("VSync", &vsync))
			{
				if (vsync)
					SDL_GL_SetSwapInterval(1);
				else
					SDL_GL_SetSwapInterval(0);
			}
		}
		if (ImGui::CollapsingHeader("Editor Camera"))
		{
			ImGui::SliderFloat("FOV", &App->camera->editorCamera.vFov, 10, 180);
			ImGui::SliderFloat("Z Near", &App->camera->editorCamera.zNear, 0.01f, 20000.f);
			ImGui::SliderFloat("Z Far", &App->camera->editorCamera.zFar, 0.01f, 20000.f);
			ImGui::SliderFloat("Movement speed", &App->camera->editorCamera.moveSpeed, 0.01f, 50.f);
			ImGui::SliderFloat("Rotation speed", &App->camera->editorCamera.rotSpeed, 0.01f, 50.f);
			ImGui::SliderFloat("Zoom speed", &App->camera->editorCamera.zoomSpeed, 0.01f, 50.f);
			App->camera->editorCamera.RecalculateFrustum();
		}
		if (ImGui::CollapsingHeader("Render Module"))
		{
			if (ImGui::Button("Show forward rendering active uniform data"))
			{
				GLint count;
				GLint size; 
				GLenum type; 
				const GLsizei bufSize = 1024; 
				GLchar name[bufSize]; 
				GLsizei length; 
				glGetProgramiv(*App->program->forwardRenderingProgram, GL_ACTIVE_UNIFORMS, &count);
				LOG("Active Uniforms: %d", count);

				for (int i = 0; i < count; i++)
				{
					glGetActiveUniform(*App->program->forwardRenderingProgram, (GLuint)i, bufSize, &length, &size, &type, name);
					LOG("Uniform #%d Type: %u Name: %s", i, type, name);
				}
			}

			ImGui::Checkbox("Frustum culling", &App->renderer->frustumCulling);			
			if (ImGui::Checkbox("Wireframe", &wireframe)) 
			{
				if (wireframe)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			ImGui::Separator();
			
			ImGui::Checkbox("Distance Fog", &App->renderer->fog);
			ImGui::ColorEdit3("Fog color", &App->renderer->fogColor[0]);
			ImGui::SliderFloat("Fog falloff", &App->renderer->fogFalloff, 1, 10000 * App->appScale);
			ImGui::SliderFloat("Fog Quadratic factor", &App->renderer->fogQuadratic, 1, 20 * App->appScale);
			
		}
		if (ImGui::CollapsingHeader("Textures Module"))
		{
			if (ImGui::Button("Clean"))
			{
				App->textures->CleanUp();
			}
			unsigned id = 0;
			for (std::list<unsigned>::iterator it = App->textures->textures.begin(); it != App->textures->textures.end(); ++it)
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
						App->textures->UnLoad(*it);						
					}
					ImGui::Separator();
					ImGui::Separator();
					ImGui::Separator();
				}
				if (App->textures->textures.size() == 0)
					break;
			}
		}
		
		if (ImGui::CollapsingHeader("KDTree Static Space Partitioning"))
		{
			ImGui::Checkbox("Show KDTree", &App->spacePartitioning->kDTree.showOnEditor);
			if (ImGui::InputInt("KDTree Max Depth", &App->spacePartitioning->kDTree.maxDepth))
			{
				if (App->spacePartitioning->kDTree.maxDepth < 1)
					App->spacePartitioning->kDTree.maxDepth = 1;
				App->spacePartitioning->kDTree.Init();
				App->spacePartitioning->kDTree.Calculate();
			}
			if (ImGui::InputInt("KDTree Bucket Size", &App->spacePartitioning->kDTree.bucketSize))
			{
				if (App->spacePartitioning->kDTree.bucketSize < 1)
					App->spacePartitioning->kDTree.bucketSize = 1;
				App->spacePartitioning->kDTree.Init();
				App->spacePartitioning->kDTree.Calculate();
			}
		}
		if (ImGui::CollapsingHeader("AABBTree Dynamic Space Partitioning"))
		{
			ImGui::Checkbox("Show AABBTree", &App->spacePartitioning->aabbTree.showOnEditor);			
		}
		ImGui::PushID(1);
		if (ImGui::CollapsingHeader("AABBTree Dynamic lighting Space Partitioning"))
		{
			ImGui::Checkbox("Show AABBTree", &App->spacePartitioning->aabbTreeLighting.showOnEditor);
		}
		ImGui::PopID();
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



