#include "SubModuleEditorInspector.h"
#include "SubModuleEditorWorldInspector.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "GameObject.h"


void SubModuleEditorInspector::Show()
{
	if (enabled)
	{		
		ImGui::Begin(editorModuleName.data(), &enabled);
		if (App->editor->worldInspector->selected != nullptr)
		{
			GameObject* selected = App->editor->worldInspector->selected;
			ImGui::Text(selected->name.c_str());
					
			ImGui::PushID(1);
			if (ImGui::InputFloat3("Position", &selected->transform->position.x))
			{
				selected->transform->RecalcModelMatrix();
			}
			ImGui::PopID();
			ImGui::PushID(2);
			if (ImGui::SliderFloat3("Rotation", &selected->transform->rotation.x, -360.f, 360.f))
			{
				selected->transform->RecalcModelMatrix();
			}
			ImGui::PopID();
			ImGui::PushID(3);
			if (ImGui::InputFloat3("Scale", &selected->transform->scale.x))
			{
				selected->transform->RecalcModelMatrix();
			}
			ImGui::PopID();

		}
		ImGui::End();
	}
}


