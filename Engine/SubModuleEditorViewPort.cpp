#define PROCESS_PRIMITIVE(){\
								newMesh->material->owner = newGO;\
								App->renderer->insertRenderizable(newGO);\
								newMesh->SendToGPU();\
								newGO->InsertComponent(newMesh);\
								newGO->isInstantiated = true; \
								App->scene->InsertGameObject(newGO);\
								App->scene->AttachToRoot(newGO);\
								newGO->transform->PropagateTransform();\
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
#include "ModuleSpacePartitioning.h"
#include "debugdraw.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "GameObject.h"
#include "ModuleRender.h"
#include "ImGuizmo/ImGuizmo.h"
#include "MathGeoLib/include/Math/MathFunc.h"
#include "SceneImporter.h"


void SubModuleEditorViewPort::Show()
{
	if (enabled)
	{	
		ImGui::Begin(editorModuleName.data(), &enabled);
		
		const char* items[] = { "Translate", "Rotate", "Scale"};
		static const char* item_current = items[0];
		static unsigned selected = 0u;		
		ImGui::PushItemWidth(100.f);
		if (ImGui::BeginCombo("Operation", item_current))
		{			
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (item_current == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current = items[n];
					selected = n;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();								
			}						
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();				

		cursorIn = ImGui::IsWindowHovered();

		if (App->scene->selected != nullptr && !App->scene->IsRoot(App->scene->selected) && App->scene->selected->isInstantiated)
		{

			ImGuiIO& io = ImGui::GetIO();
			ImVec2 winPos = ImGui::GetWindowPos();
			ImVec2 winSize = ImGui::GetContentRegionMax();			
			
			float4x4 view = App->camera->editorCamera.frustum.ViewMatrix();
			
			App->camera->guizmoLock = ImGuizmo::IsOver();
						
			ImGuizmo::SetRect(winPos.x, winPos.y, winSize.x, winSize.y);
			float4x4 modelMatrix;
			if (selected == ImGuizmo::OPERATION::ROTATE)
			{
				modelMatrix = App->scene->selected->transform->modelMatrixLocal;
				float3 originalPos = modelMatrix.Col3(3);
				modelMatrix.SetCol3(3, App->scene->selected->transform->modelMatrixGlobal.Col3(3)); //move guizmo to pivot world coordinates
				modelMatrix.Transpose();				
				ImGuizmo::Manipulate(view.Transposed().ptr(), App->camera->editorCamera.frustum.ProjectionMatrix().Transposed().ptr(), (ImGuizmo::OPERATION)selected, ImGuizmo::LOCAL, modelMatrix.ptr());								
				if (ImGuizmo::IsUsing())
				{
					modelMatrix.Transpose();
					modelMatrix.SetCol3(3, originalPos); //restore local position
					App->scene->selected->transform->modelMatrixLocal = modelMatrix;
					App->scene->selected->transform->ExtractLocalTransformFromMatrix();
					App->scene->selected->transform->PropagateTransform();
				}
				
			}
			else
			{
				modelMatrix = App->scene->selected->transform->modelMatrixGlobal;
				modelMatrix.Transpose();
				float4x4 deltaMatrix;
				ImGuizmo::Manipulate(view.Transposed().ptr(), App->camera->editorCamera.frustum.ProjectionMatrix().Transposed().ptr(), (ImGuizmo::OPERATION)selected, ImGuizmo::WORLD, modelMatrix.ptr(), deltaMatrix.ptr());				
				if (ImGuizmo::IsUsing())
				{					
					deltaMatrix.Transpose();
					float3 translation;
					float3 scale;
					float3x3 rot;
					deltaMatrix.Decompose(translation, rot, scale);
					if (selected == ImGuizmo::OPERATION::SCALE)
					{
						App->scene->selected->transform->SetScale(scale);
					}
					else
						App->scene->selected->transform->Translate(translation);

					App->scene->selected->transform->PropagateTransform();
				}
			}
			
		}
				
		ImVec2 size = ImGui::GetWindowSize();			
		ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 60); //padding
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
			dd::xzSquareGrid(-200.f * App->appScale, 200.f * App->appScale, 0.f, 1.f * App->appScale, dd::colors::DarkGray);
			if (App->scene->sceneCamera != nullptr && App->scene->selected == App->scene->sceneCamera->owner)
			{
				dd::frustum((App->scene->sceneCamera->frustum.ProjectionMatrix() * App->scene->sceneCamera->frustum.ViewMatrix()).Inverted(), dd::colors::Coral);
			}
			if (App->scene->selected != nullptr && !App->scene->selected->isContainer && !App->scene->IsRoot(App->scene->selected) && App->scene->selected->aaBBGlobal != nullptr)
			{
				dd::aabb(App->scene->selected->aaBBGlobal->minPoint, App->scene->selected->aaBBGlobal->maxPoint, dd::colors::Yellow);				
			}
			App->debugDraw->Draw(&App->camera->editorCamera, App->frameBuffer->framebuffer, App->frameBuffer->viewPortWidth, App->frameBuffer->viewPortHeight);
			
		}		
		App->spacePartitioning->quadTree.DebugDraw();
		App->frameBuffer->UnBind();
		ImGui::Image((void*)(intptr_t)App->frameBuffer->texColorBuffer, viewPortRegion, ImVec2(0,1), ImVec2(1,0));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMPORTED_GAMEOBJECT_ID"))
			{
				char movedId[40];
				sprintf_s(movedId, (char*)payload->Data); //TODO: use constant to 40
				GameObject* movedGO = App->scene->FindInstanceOrigin(movedId);
				if (movedGO != nullptr)
				{
					GameObject* instance = movedGO->MakeInstanceOf();
					instance->transform->PropagateTransform();
					instance->SetInstanceOf(movedGO->gameObjectUUID);
					App->scene->AttachToRoot(instance);
				}
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

