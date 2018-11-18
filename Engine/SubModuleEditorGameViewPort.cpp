#include "SubModuleEditorGameViewPort.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ComponentCamera.h"
#include "imgui/imgui.h"

#define IMAGE_SIZE 500 

void SubModuleEditorGameViewPort::Show()
{
	if (enabled)
	{	
		ImGui::Begin("Game");
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 30); //padding
		if (width != size.x || height != size.y) //viewport changed
		{					
			width = size.x;
			height = size.y;
			App->frameBuffer->gameViewPortWidth = viewPortRegion.x;
			App->frameBuffer->gameViewPortHeight = viewPortRegion.y;			
		}
		if (App->scene->sceneCamera == nullptr)
		{
			unsigned xPos = ImGui::GetWindowContentRegionMax().x / 2 - IMAGE_SIZE / 2;
			unsigned yPos = ImGui::GetWindowContentRegionMax().y / 2 - IMAGE_SIZE / 2;
			ImGui::SetCursorPos(ImVec2(xPos, yPos));
			ImGui::Image((void*)(intptr_t)App->editor->noCamTex, ImVec2(IMAGE_SIZE, IMAGE_SIZE), ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			App->scene->sceneCamera->aspectRatio = viewPortRegion.x / viewPortRegion.y;
			App->scene->sceneCamera->RecalculateFrustum();
			App->frameBuffer->RecalcFrameBufferTexture();

			ImGui::Image((void*)(intptr_t)App->frameBuffer->gameTexColorBuffer, viewPortRegion, ImVec2(1, 1), ImVec2(0, 0));
		}
		ImGui::End();
	}
}

