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
#include "ComponentMesh.h"
#include "GameObject.h"
#include "ModuleRender.h"
#include "SceneImporter.h"


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
		App->renderer->Render(&App->camera->editorCamera);
		if (App->editor->gizmosEnabled)
		{
			dd::xzSquareGrid(-200.f, 200.f, 0.f, 1.f, dd::colors::DarkGray);
			if (App->scene->sceneCamera != nullptr && App->scene->selected == App->scene->sceneCamera->owner)
			{
				dd::frustum((App->scene->sceneCamera->frustum.ProjectionMatrix() * App->scene->sceneCamera->frustum.ViewMatrix()).Inverted(), dd::colors::Coral);
			}
			if (App->scene->selected != nullptr)
			{
				dd::aabb(App->scene->selected->aaBBGlobal.minPoint, App->scene->selected->aaBBGlobal.maxPoint, dd::colors::Yellow);				
			}
			App->debugDraw->Draw(&App->camera->editorCamera, App->frameBuffer->framebuffer, App->frameBuffer->viewPortWidth, App->frameBuffer->viewPortHeight);
			
		}		
		App->frameBuffer->UnBind();
		ImGui::Image((void*)(intptr_t)App->frameBuffer->texColorBuffer, viewPortRegion, ImVec2(0,1), ImVec2(1,0));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMPORTED_OK"))
			{
				void* data = (void*)payload->Data;
				SceneImporter si;
				char filename[4096];				
				memcpy(&filename[0], data, payload->DataSize);				
				unsigned i = 4;
				char fileNameClean[4096];
				while (filename[++i] > 0);
				memcpy(&fileNameClean[0], &filename[4], i - 4);
				fileNameClean[i - 4] = 0;
				GameObject* loaded = si.Load(std::string(fileNameClean) + ".dmd");
				App->scene->insertGameObject(nullptr);
			}
		}
		if (ImGui::BeginPopupContextItem("Editor"))
		{
			if (ImGui::TreeNodeEx("Add primitive"))
			{				
				if (ImGui::Button("Cube", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Cube");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::CUBE);
					App->renderer->insertRenderizable(newMesh);
					newMesh->SendToGPU();
					newGO->InsertComponent(newMesh);
					App->scene->insertGameObject(newGO);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Sphere", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Sphere");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::SPHERE);
					App->renderer->insertRenderizable(newMesh);
					newMesh->SendToGPU();
					newGO->InsertComponent(newMesh);
					App->scene->insertGameObject(newGO);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Torus", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Torus");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::TORUS);
					App->renderer->insertRenderizable(newMesh);
					newMesh->SendToGPU();
					newGO->InsertComponent(newMesh);
					App->scene->insertGameObject(newGO);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Cylinder", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Cylinder");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::CYLINDER);
					App->renderer->insertRenderizable(newMesh);
					newMesh->SendToGPU();
					newGO->InsertComponent(newMesh);
					App->scene->insertGameObject(newGO);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Plane", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Plane");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::PLANE);
					App->renderer->insertRenderizable(newMesh);
					newMesh->SendToGPU();
					newGO->InsertComponent(newMesh);
					App->scene->insertGameObject(newGO);
					ImGui::CloseCurrentPopup();
				}
				ImGui::TreePop();
			}
			ImGui::EndPopup();
		}
		ImGui::End();
	}
}

