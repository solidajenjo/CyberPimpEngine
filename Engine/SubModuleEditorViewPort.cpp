#include "SubModuleEditorViewPort.h"
#include "Application.h"
#include "ModuleRender.h"
#include "imgui.h"


SubModuleEditorViewPort::SubModuleEditorViewPort(char* editorModuleName) : SubModuleEditor(editorModuleName)
{	

}


SubModuleEditorViewPort::~SubModuleEditorViewPort()
{
	
}

void SubModuleEditorViewPort::Show()
{
	if (enabled)
	{	
		//TODO::Flip texture
		ImGui::Begin("viewport");
		{
			ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 30); //padding
			ImGui::Image((void*)(intptr_t)App->renderer->texColorBuffer, viewPortRegion, ImVec2(1,1), ImVec2(0,0));
			App->renderer->viewPortWidth = viewPortRegion.x;
			App->renderer->viewPortHeight = viewPortRegion.y;
			ImGui::End();			
		}
		
	}
}

