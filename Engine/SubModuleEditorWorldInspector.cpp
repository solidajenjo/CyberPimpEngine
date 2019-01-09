#include "SubModuleEditorWorldInspector.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "Transform.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"


void SubModuleEditorWorldInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		
		if (ImGui::Button("New"))
		{
			ImGui::OpenPopup("Editor");
		}
		if (ImGui::BeginPopup("Editor"))
		{
			if (ImGui::Button("Empty GameObject"))
			{
				GameObject* newGO = new GameObject("Empty");
				newGO->transform->PropagateTransform();
				App->scene->InsertGameObject(newGO);
				App->scene->AttachToRoot(newGO);
				newGO->isInstantiated = true;
				ImGui::CloseCurrentPopup();
			}
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
		App->scene->ShowHierarchy();
		ImGui::End();
	}
}


