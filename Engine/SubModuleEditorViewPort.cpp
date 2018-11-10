#include "SubModuleEditorViewPort.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ModuleCamera.h"
#include "imgui/imgui.h"


void SubModuleEditorViewPort::Show()
{
	if (enabled)
	{	
		ImGui::Begin("Viewport");
		
		cursorIn = ImGui::IsWindowHovered();
		ImVec2 size = ImGui::GetWindowSize();			
		ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 30); //padding
		if (width != size.x || height != size.y) //viewport changed
		{					
			width = size.x;
			height = size.y;
			App->frameBuffer->viewPortWidth = viewPortRegion.x;
			App->frameBuffer->viewPortHeight = viewPortRegion.y;
			App->camera->aspectRatio = viewPortRegion.x / viewPortRegion.y;
			App->camera->RecalculateFrustum();
			App->frameBuffer->RecalcFrameBufferTexture();
		}
		ImGui::Image((void*)(intptr_t)App->frameBuffer->texColorBuffer, viewPortRegion, ImVec2(1,1), ImVec2(0,0));									
		
		ImGui::End();
	}
}

