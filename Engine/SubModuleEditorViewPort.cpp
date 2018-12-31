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
#include "ModuleWindow.h"
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
#include "SDL/include/SDL_mouse.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"
#include "ModuleInput.h"

#include "TimeClock.h"

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

		if (App->scene->selected != nullptr && App->scene->selected->isInstantiated)
		{
			ImGui::SameLine();
			float distance = App->scene->selected->transform->getGlobalPosition().Distance(App->camera->editorCamera.camPos);
			ImGui::Text("Distance from selected %.3f", distance);
		}
		cursorIn = ImGui::IsWindowHovered();

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
		if (App->spacePartitioning->quadTree.showOnEditor)
			App->spacePartitioning->quadTree.DebugDraw();		
		if (App->spacePartitioning->kDTree.showOnEditor)
			App->spacePartitioning->kDTree.DebugDraw();
		//mouse picking		
		
		App->frameBuffer->UnBind();
		ImVec2 curPos = ImGui::GetCursorPos();
		ImVec2 winPos = ImGui::GetWindowPos();
		ImGui::Image((void*)(intptr_t)App->frameBuffer->texColorBuffer, viewPortRegion, ImVec2(0,1), ImVec2(1,0));
		if (ImGui::IsItemHovered())
		{
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && !ImGuizmo::IsUsing())
			{
				LineSegment	picking;
				ImVec2 mousePos = ImGui::GetMousePos();				
				ImVec2 mouseInWindowPos = ImVec2(mousePos.x - winPos.x - curPos.x, mousePos.y - winPos.y - curPos.y); //(0,0) is upper-left of the framebuffer
				float x = Lerp(-1.f, 1.f, mouseInWindowPos.x / App->frameBuffer->viewPortWidth); // -1 -> left // 1 right
				float y = Lerp(1.f, -1.f, mouseInWindowPos.y / App->frameBuffer->viewPortHeight); // 1 -> up // -1 down
				picking = App->camera->editorCamera.frustum.UnProjectLineSegment(x, y); //x & y in clipping coords
				std::set<GameObject*> intersections;
				App->spacePartitioning->quadTree.GetIntersections(picking, intersections);
				//TimeClock tc;
				//tc.StartMS();				
				//LOG("Calculate %.3f", tc.ReadMS());
				App->spacePartitioning->kDTree.GetIntersections(picking, intersections);
				//LOG("Intersections %.3f", tc.ReadMS());
				float3 bestHitPoint = float3::inf;
				for each (GameObject* go in intersections)
				{
					LineSegment localLS = picking;
					localLS.Transform(go->transform->modelMatrixGlobal.Inverted());
					float3 hitPoint;
					if (go->RayAgainstMeshNearestHitPoint(localLS, hitPoint) && localLS.a.Distance(hitPoint) < localLS.a.Distance(bestHitPoint))
					{
						App->scene->selected = go;
						bestHitPoint = hitPoint;						
					}

				}
				//float kdTime = tc.ReadMS();

				//LOG("KDtree %.3f Checks %d", kdTime, intersections.size());
			}
		}
		if (App->scene->selected != nullptr && !App->scene->IsRoot(App->scene->selected) && App->scene->selected->isInstantiated)
		{

			ImGuiIO& io = ImGui::GetIO();
			ImVec2 winPos = ImGui::GetWindowPos();
			ImVec2 winSize = ImGui::GetContentRegionMax();

			float4x4 view = App->camera->editorCamera.frustum.ViewMatrix();

			App->camera->guizmoLock = ImGuizmo::IsOver();

			ImGuizmo::SetRect(winPos.x, winPos.y, winSize.x, winSize.y);
			ImGuizmo::SetDrawlist();
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
					float3 rotation = App->scene->selected->transform->rotation;
					App->scene->selected->transform->modelMatrixGlobal = modelMatrix.Transposed();
					App->scene->selected->transform->NewAttachment(); //recalculate local translation & scale
					App->scene->selected->transform->rotation = rotation; //restore local rotation
					App->scene->selected->transform->RecalcModelMatrix(); //recalculate local model matrix
					App->scene->selected->transform->PropagateTransform();
				}
			}

		}
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
		/*if (ImGui::BeginPopupContextItem("Editor"))
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
		}*/
		ImGui::End();
	}
}

