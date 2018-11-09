#include "SubModuleEditorViewPort.h"
#include "Application.h"
#include "ModuleRender.h"
#include "imgui/imgui.h"


void SubModuleEditorViewPort::Show()
{
	if (enabled)
	{	
		ImGui::Begin("Viewport");
		{
			ImVec2 size = ImGui::GetWindowSize();
			if (width != size.x || height != size.y)
			{		
				width = size.x;
				height = size.y;
				//TODO: Call Framebuffer module
			}
			ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 30); //padding
			ImGui::Image((void*)(intptr_t)App->renderer->texColorBuffer, viewPortRegion, ImVec2(1,1), ImVec2(0,0));
			App->renderer->viewPortWidth = viewPortRegion.x;
			App->renderer->viewPortHeight = viewPortRegion.y;
			ImGui::End();			
		}
		
	}
}

