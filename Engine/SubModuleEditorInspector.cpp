#include "SubModuleEditorInspector.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentCamera.h"
#include <list>


void SubModuleEditorInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		if (App->scene->selected != nullptr)
		{
			if (App->scene->IsRoot(App->scene->selected)) //root node unnaccesible to edit
			{
				ImGui::End();
				return;
			}
			App->scene->selected->transform->EditorDraw();			
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();	
			bool firstMesh = false; //only one mesh drawing per gameobject
			ImGui::InputText("Name", &App->scene->selected->name[0], 40);
			if (ImGui::Combo("Add component", &selectedNewComponent, "Camera\0Dummy\0")) {
				if (selectedNewComponent == (int)Component::ComponentTypes::CAMERA_COMPONENT)
				{
					ComponentCamera* newCam = new ComponentCamera(false);
					newCam->RecalculateFrustum(App->scene->selected->transform->front, App->scene->selected->transform->up);
					newCam->owner = App->scene->selected;
					App->scene->selected->components.push_back(newCam);
				}
			}
			for (std::list<Component*>::const_iterator it = App->scene->selected->components.begin(); it != App->scene->selected->components.end(); ++it)
			{							
				if (strcmp((*it)->type, "Mesh") != 0)
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
							if (strcmp((*it2)->type, "Mesh") == 0)
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


