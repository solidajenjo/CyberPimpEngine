#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "imgui/imgui.h"

void ComponentMesh::EditorDraw()
{
	//show info from his material
	material->EditorDraw();
}
