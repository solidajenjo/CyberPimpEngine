#include "SubModuleEditorMenu.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleSpacePartitioning.h"
#include "AABBTree.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "FileExplorer.h"
#include <vector>


SubModuleEditorMenu::SubModuleEditorMenu(const std::string & editorModuleName) : SubModuleEditor(editorModuleName)
{
	fileExplorer = new FileExplorer();
}

SubModuleEditorMenu::~SubModuleEditorMenu()
{
	RELEASE(fileExplorer);
}

void SubModuleEditorMenu::Show()
{
	bool openFileExplorer = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File")) 
		{
			if (ImGui::TreeNode("Scene"))
			{
				if (ImGui::MenuItem("New"))
				{
					App->scene->CleanUp();
					App->scene->Init();	
					App->spacePartitioning->aabbTree.CleanUp();
					App->spacePartitioning->aabbTree.Init(GameObject::GameObjectLayers::WORLD_VOLUME);
					App->spacePartitioning->aabbTreeLighting.Init(GameObject::GameObjectLayers::LIGHTING);
				}
				if (ImGui::MenuItem("Save"))
				{
					openFileExplorer = true;
					fileExplorer->Reset();
					currentOperation = MenuOperations::SAVE;					
				}
				if (ImGui::MenuItem("Load"))
				{
					openFileExplorer = true;
					fileExplorer->Reset();
					currentOperation = MenuOperations::LOAD;					
				}				
				ImGui::TreePop();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
				App->exit = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("SubModules")) {
			for (std::vector<SubModuleEditor*>::iterator it = App->editor->subModules.begin() + 1; it != App->editor->subModules.end(); ++it)
			{
				ImGui::Checkbox((*it)->editorModuleName.c_str(), &(*it)->enabled);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {

			if (ImGui::MenuItem("Documentation"))
				ShellExecute(NULL, "open", "https://github.com/solidajenjo/DraconisEngine/wiki",
					NULL, NULL, SW_SHOWNORMAL);
			if (ImGui::MenuItem("Download latest"))
				ShellExecute(NULL, "open", "https://github.com/solidajenjo/DraconisEngine/releases",
					NULL, NULL, SW_SHOWNORMAL);
			if (ImGui::MenuItem("Report a bug"))
				ShellExecute(NULL, "open", "https://github.com/solidajenjo/DraconisEngine/issues",
					NULL, NULL, SW_SHOWNORMAL);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	if (openFileExplorer)
	{
		ImGui::OpenPopup(fileExplorer->title);
	}
		
	if (fileExplorer->Open())
	{
		switch (currentOperation)
		{
		case MenuOperations::SAVE:
			LOG("Save scene -> %s", fileExplorer->path.c_str());	
			App->scene->SaveScene(fileExplorer->rawPath);
			currentOperation = MenuOperations::NONE;
			break;
		case MenuOperations::LOAD:
			LOG("Load scene -> %s", fileExplorer->path.c_str()); 
			App->scene->LoadScene(fileExplorer->path);
			currentOperation = MenuOperations::NONE;
			break;
		}

	}		
	
}
