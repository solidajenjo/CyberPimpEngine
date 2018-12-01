#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ComponentMesh.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "SDL/include/SDL_rwops.h"
#include "SDL/include/SDL_filesystem.h"


bool ModuleScene::Init()
{
	LOG("Init Scene module");
	root = new GameObject("SceneRoot");
	root->name = "Scene";
	directory = new GameObject("ImportedFiles");
	directory->name = ".";
	return true;
}

bool ModuleScene::CleanUp()
{
	LOG("Cleaning scene GameObjects.");
	for (std::vector<GameObject*>::reverse_iterator it = importedGameObjects.rbegin(); it != importedGameObjects.rend(); ++it) //must be cleaned backwards because dependencies
	{
		RELEASE(*it);
	}
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

void ModuleScene::InsertGameObject(GameObject * newGO)
{
	assert(newGO != nullptr);
	root->children.push_back(newGO);
	newGO->parent = root;
	FlattenHierarchy(newGO);
}

void ModuleScene::ImportGameObject(GameObject * newGO)
{
	assert(newGO != nullptr);
	directory->children.push_back(newGO);
	FlattenImported(newGO);
	newGO->parent = directory;
}

void ModuleScene::DestroyGameObject(GameObject * destroyableGO)
{
	assert(destroyableGO != nullptr);
}

void ModuleScene::ShowHierarchy(bool isWorld)
{
	if (isWorld)
	{
		assert(root != nullptr);
		DrawNode(root);
	}
	else
	{
		assert(directory != nullptr);
		DrawNode(directory, isWorld);
	}
}

void ModuleScene::DrawNode(GameObject* gObj, bool isWorld)
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
	bool node_open = ImGui::TreeNodeEx(gObj->name.c_str(), flags);
	if (isWorld)
	{
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
				sprintf_s(movedId, (char*)payload->Data); //TODO: use constant to 40
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
	}
	else
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("IMPORTED_GAMEOBJECT_ID", &gObj->gameObjectUUID, sizeof(char) * 40); //TODO: use constant to 40
			ImGui::EndDragDropSource();
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
				DrawNode(*it, isWorld);
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

const std::vector<GameObject*>* ModuleScene::getImportedGameObjects() const
{
	return &importedGameObjects;
}

bool ModuleScene::IsRoot(const GameObject * go) const
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

void ModuleScene::FlattenHierarchy(GameObject* go) //TODO:Remove recursivity
{
	sceneGameObjects.push_back(go);
	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
	{
		for (std::list<Component*>::iterator it2 = go->components.begin(); it2 != go->components.end(); ++it2)
		{
			if (strcmp((*it2)->type, "Mesh") == 0) //todo replace with enum
			{
				ComponentMesh* mesh = (ComponentMesh*)(*it2);
				mesh->SendToGPU();
				App->renderer->insertRenderizable(mesh);
			}
		}
		FlattenHierarchy(*it);
	}

}

void ModuleScene::FlattenImported(GameObject * go)
{
	importedGameObjects.push_back(go);
	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
		FlattenImported(*it);
}

void ModuleScene::Serialize()
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	root->Serialize(writer);
	const char* jsonContent = new char[sb.GetSize()];
	jsonContent = sb.GetString();
	//save scene
	SDL_RWops *rw = SDL_RWFromFile("scene.dsc", "w");
	if (rw == nullptr)
	{
		LOG("Couldn't save scene. %s", SDL_GetError());
	}
	else
	{
		if (SDL_RWwrite(rw, jsonContent, strlen(jsonContent), 1) == 1)
		{
			LOG("Scene saved.");
		}
		else
		{
			LOG("Couldn't save scene. %s", SDL_GetError());
		}
		SDL_RWclose(rw);
	}
}
