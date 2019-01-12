#include "SubModuleEditorFileInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "MaterialImporter.h"
#include "Transform.h"

SubModuleEditorFileInspector::SubModuleEditorFileInspector(const std::string & editorModuleName) : SubModuleEditor(editorModuleName)
{
	
}

void SubModuleEditorFileInspector::Show()
{
	ImGui::Begin("Files");
	if (ImGui::Button("New"))
	{
		ImGui::OpenPopup("Editor");
	}
	if (ImGui::BeginPopup("Editor"))
	{
		if (ImGui::Button("Material"))
		{
			GameObject* mat = new GameObject("Material", true);
			MaterialImporter mi;	
			ComponentMaterial* newMat = new ComponentMaterial(1.f, 1.f, 1.f, 1.f);
			std::string matPath = "Library/Materials/" + std::string(mat->gameObjectUUID) + ".mat";
			sprintf_s(newMat->materialPath, matPath.c_str());
			mi.Save(matPath.c_str(), newMat);
			RELEASE(newMat);
			mat->InsertComponent(ComponentMaterial::GetMaterial(matPath));			
			RELEASE(mat->transform);
			App->scene->ImportGameObject(mat, ModuleScene::ImportedType::MATERIAL); //import material
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	App->scene->ShowHierarchy(false);
	ImGui::End();
}

