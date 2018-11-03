#include "SubModuleEditorWorldInspector.h"
#include "Application.h"
#include "ModuleWorld.h"
#include "Entity.h"
#include "EntityMesh.h"
#include "imgui.h"


SubModuleEditorWorldInspector::SubModuleEditorWorldInspector(char* editorModuleName) : SubModuleEditor(editorModuleName)
{	

}


SubModuleEditorWorldInspector::~SubModuleEditorWorldInspector()
{
}

void SubModuleEditorWorldInspector::Show()
{
	if (enabled)
	{
		ImGui::Begin(editorModuleName, &enabled);
		if (ImGui::TreeNode("World"))
		{
			for (unsigned i = 0; i < App->world->worldEntities.size(); ++i)
			{
				EntityMesh* em = (EntityMesh*)App->world->worldEntities[i];
				if (ImGui::TreeNode(App->world->worldEntities[i]->entityName))
				{
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}
}
