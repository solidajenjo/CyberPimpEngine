#include "SubModuleEditorInspector.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include <list>

void SubModuleEditorInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		if (App->scene->selected != nullptr)
		{
			if (App->scene->isRoot(App->scene->selected)) //root node unnaccesible to edit
			{
				ImGui::End();
				return;
			}
			App->scene->selected->transform->EditorDraw();			
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();	
			bool firstMesh = false;
			for (std::list<Component*>::const_iterator it = App->scene->selected->components.begin(); it != App->scene->selected->components.end(); ++it)
			{							
				if ((*it)->type != "Mesh")
				{
					if (ImGui::Button("Remove"))
					{
						//delete component
					}
					(*it)->EditorDraw();
				}
				else if (!firstMesh) //if it's a mesh a different behaviour needed when multi-mesh gameobjects
				{
					firstMesh = true;
					ImGui::PushID(this);
					if (ImGui::Button("Remove"))
					{
						//delete component
					}
					if (ImGui::CollapsingHeader("Mesh"))
					{
						for (std::list<Component*>::const_iterator it2 = App->scene->selected->components.begin(); it2 != App->scene->selected->components.end(); ++it2)
						{
							if ((*it2)->type == "Mesh")
								(*it2)->EditorDraw();
						}
					}
					ImGui::PopID();
				}
			}

		}
		ImGui::End();
	}
}


