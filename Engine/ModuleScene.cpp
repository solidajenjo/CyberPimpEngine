#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"


bool ModuleScene::Init()
{
	LOG("Init Scene module");
	root = new GameObject("Scene");
	return true;
}

update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Cleaning scene GameObjects.");
	for (std::vector<GameObject*>::reverse_iterator it = sceneGameObjects.rbegin(); it != sceneGameObjects.rend(); ++it) //must be cleaned backwards because dependencies
	{
		RELEASE(*it);
	}
	sceneGameObjects.resize(0);
	root->children.clear();
	LOG("Cleaning scene GameObjects. Done");
	selected = nullptr;
	sceneCamera = nullptr;
	return true;
}

void ModuleScene::insertGameObject(GameObject * newGO)
{
	assert(newGO != nullptr);
	root->children.push_back(newGO);
	newGO->parent = root;
	flattenHierarchy(newGO);
}

void ModuleScene::destroyGameObject(GameObject * destroyableGO)
{
	assert(destroyableGO != nullptr);
}

void ModuleScene::showHierarchy()
{
	drawNode(root);
}

void ModuleScene::drawNode(GameObject* gObj)
{
	assert(gObj != nullptr);

	ImGuiTreeNodeFlags flags;
	if (gObj->children.size() == 0)
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
	else
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	ImGui::PushID(&gObj);
	if (gObj->selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	ImGui::PushID(&gObj->name);
	bool node_open = ImGui::TreeNodeEx(gObj->name, flags);
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("GAMEOBJECT_ID", &gObj->gameObjectUUID, sizeof(char) * 40); //TODO: use constant to 40
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_ID"))
		{
			char movedId[40];
			sprintf_s(movedId,(char*)payload->Data); //TODO: use constant to 40
			GameObject* movedGO = nullptr;
			for (std::vector<GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
			{
				if (strcmp((*it)->gameObjectUUID, movedId) == 0)
				{
					movedGO = *it;
					break;
				}
			}
			movedGO->parent->children.remove(movedGO);

			movedGO->parent = gObj;
			LOG("Moved gameobject %s", gObj->gameObjectUUID);
			gObj->children.push_back(movedGO);			
			movedGO->transform->NewAttachment();
		}
	}
	if (ImGui::IsItemClicked())
	{
		gObj->selected = !gObj->selected;
		if (gObj->selected && selected != gObj)
		{
			selected != nullptr ? selected->selected = false : 1;
			selected = gObj;			
		}
		else if (!gObj->selected && selected == gObj)
		{
			selected = nullptr;
		}
	}
	if (node_open)
	{
		if (gObj->children.size() > 0)
		{
			for (std::list<GameObject*>::iterator it = gObj->children.begin(); it != gObj->children.end(); ++it)
				drawNode(*it);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::PopID();

}

const std::vector<GameObject*>* ModuleScene::getSceneGameObjects() const
{
	return &sceneGameObjects;
}

bool ModuleScene::isRoot(const GameObject * go) const
{
	return go == root;
}

void ModuleScene::SetSkyBox()
{
	/*
	if (App->frameBuffer->skyBox == nullptr)
	{		
		assert(sceneGameObjects.size() == 2);
		assert(sceneGameObjects.back()->components.size() == 1);
		App->frameBuffer->skyBox = (ComponentMesh*)sceneGameObjects.back()->components.front(); //store the skybox mesh on framebuffer module on start. Released there
		sceneGameObjects.front()->components.clear();
		sceneGameObjects.clear(); 
		root->children.clear();
		App->textures->textures.clear();
		App->renderer->renderizables.clear(); //Clear renderizables, render skybox on demand only
	}
	*/
}

void ModuleScene::flattenHierarchy(GameObject* go) //TODO:Remove recursivity
{
	sceneGameObjects.push_back(go);
	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
		flattenHierarchy(*it);

}

void ModuleScene::Serialize()
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	root->Serialize(writer);
	const char* jsonContent = new char[sb.GetSize()];
	jsonContent = sb.GetString();
	int x = 0;
}
