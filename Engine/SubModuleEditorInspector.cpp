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
			App->scene->selected->transform->EditorDraw();
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();			
			for (std::list<Component*>::const_iterator it = App->scene->selected->components.begin(); it != App->scene->selected->components.end(); ++it)
			{
				if (ImGui::Button("Remove"))
				{
					//delete component
				}
				(*it)->EditorDraw();
			}

		}
		ImGui::End();
	}
}


