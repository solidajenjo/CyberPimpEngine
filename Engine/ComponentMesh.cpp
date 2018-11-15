#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "imgui/imgui.h"

void ComponentMesh::EditorDraw()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (ImGui::Button("Use checkers"))
		{
			texture = App->editor->checkersTex;
		}
		ImGui::Text("Put here the materials of the mesh");
	}
}
