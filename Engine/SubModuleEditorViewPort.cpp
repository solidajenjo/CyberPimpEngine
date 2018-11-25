#include "SubModuleEditorViewPort.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleEditorCamera.h"
#include "imgui/imgui.h"
#include "ModuleDebugDraw.h"
#include "ModuleProgram.h"
#include "debugdraw.h"
#include "ComponentCamera.h"
#include "ModuleRender.h"


void SubModuleEditorViewPort::Show()
{
	if (enabled)
	{	
		ImGui::Begin(editorModuleName.data(), &enabled);
		
		cursorIn = ImGui::IsWindowHovered();
		ImVec2 size = ImGui::GetWindowSize();			
		ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 30); //padding
		if (width != size.x || height != size.y) //viewport changed
		{					
			width = size.x;
			height = size.y;
			App->frameBuffer->viewPortWidth = viewPortRegion.x;
			App->frameBuffer->viewPortHeight = viewPortRegion.y;
			App->camera->editorCamera.aspectRatio = viewPortRegion.x / viewPortRegion.y;
			App->camera->editorCamera.RecalculateFrustum();
			App->frameBuffer->RecalcFrameBufferTexture();
		}
		
		App->frameBuffer->Bind();
		if (App->editor->gizmosEnabled)
		{
			dd::xzSquareGrid(-200.f, 200.f, 0.f, 1.f, dd::colors::DarkGray);
			App->debugDraw->Draw(&App->camera->editorCamera, App->frameBuffer->framebuffer, App->frameBuffer->viewPortWidth, App->frameBuffer->viewPortHeight);
			
		}
		App->renderer->Render(&App->camera->editorCamera);
		App->frameBuffer->UnBind();
		ImGui::Image((void*)(intptr_t)App->frameBuffer->texColorBuffer, viewPortRegion, ImVec2(0,1), ImVec2(1,0));	
		ImGui::End();
	}
}

