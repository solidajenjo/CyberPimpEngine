#define PROCESS_PRIMITIVE(){\
								App->renderer->insertRenderizable(newGO);\
								newMesh->SendToGPU();\
								newGO->InsertComponent(newMesh);\
								App->scene->InsertGameObject(newGO);\
								ImGui::CloseCurrentPopup(); }

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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMPORTED_GAMEOBJECT_ID"))
			{
				char movedId[40];
				sprintf_s(movedId, (char*)payload->Data); //TODO: use constant to 40
				GameObject* movedGO = nullptr;
				/*const std::vector<GameObject*>* importedGameObjects = App->scene->getImportedGameObjects();
				for (std::vector<GameObject*>::const_iterator it = importedGameObjects->begin(); it != importedGameObjects->end(); ++it)
				{
					if (strcmp((*it)->gameObjectUUID, movedId) == 0)
					{
						movedGO = *it;
						break;
					}
				}*/
				if (movedGO != nullptr)
				{
					GameObject* instance = movedGO->MakeInstanceOf();
					instance->SetInstanceOf(movedGO->gameObjectUUID);
					App->scene->InsertGameObject(instance);
				}
			}
		}
		if (ImGui::BeginPopupContextItem("Editor"))
		{
			if (ImGui::TreeNodeEx("Add primitive"))
			{				
				//TODO: Calculate AABBs
				if (ImGui::Button("Cube", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Cube");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::CUBE);
					PROCESS_PRIMITIVE();
				}
				if (ImGui::Button("Sphere", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Sphere");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::SPHERE);
					PROCESS_PRIMITIVE();
				}
				if (ImGui::Button("Torus", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Torus");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::TORUS);
					PROCESS_PRIMITIVE();
				}
				if (ImGui::Button("Cylinder", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Cylinder");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::CYLINDER);
					PROCESS_PRIMITIVE();
				}
				if (ImGui::Button("Plane", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
				{
					GameObject* newGO = new GameObject("Plane");
					ComponentMesh* newMesh = new ComponentMesh(ComponentMesh::Primitives::PLANE);
					PROCESS_PRIMITIVE();
				}
				ImGui::TreePop();
			}
			ImGui::EndPopup();
		}
		ImGui::End();
	}
}

