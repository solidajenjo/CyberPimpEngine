#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ComponentMesh.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include "SDL/include/SDL_rwops.h"
#include <algorithm>
#include "SDL/include/SDL_filesystem.h"

bool ModuleScene::Init()
{
	LOG("Init Scene module");
	SDL_RWops *rw = SDL_RWFromFile("assets.dsc", "r");
	if (rw == nullptr) //no previous assets file found
	{
		directory = new GameObject("Assets");
	}
	else
	{
		//restore imported gameobjects
		unsigned fileSize = SDL_RWsize(rw);
		char* buffer = new char[fileSize];
		if (SDL_RWread(rw, buffer, fileSize, 1) == 1)
		{
			SDL_RWclose(rw);
			rapidjson::Document document;
			if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
			{
				LOG("Error loading imported files. Imported file corrupted.");
				directory = new GameObject("Assets");
			}
			else
			{
				directory = new GameObject("Assets");
				sprintf_s(directory->gameObjectUUID, document["UUID"].GetString());
				rapidjson::Value children = document["children"].GetArray();
				bool isOk = true;
				for (rapidjson::Value::ValueIterator it = children.Begin(); it != children.End(); ++it)
				{
					GameObject* newGO = new GameObject("");
					isOk = isOk && newGO->UnSerialize(*it, false);
				}
				if (!isOk)
				{
					CleanUp();
					RELEASE(directory);
					directory = new GameObject("Assets");
					LOG("Error loading imorted files.");
				}
				else
				{
					directory->transform->PropagateTransform();
					FlattenImported(directory);
				}
			}
			delete buffer;
		}
		else
		{
			LOG("Error loading previous scene %s", SDL_GetError());
			SDL_RWclose(rw);
		}
	}

	rw = SDL_RWFromFile("scene.dsc", "r");
	if (rw == nullptr) //no previous scene found
	{
		root = new GameObject("Scene");		
	}
	else
	{
		//restore instantiated gameobjects
		unsigned fileSize = SDL_RWsize(rw);
		char* buffer = new char[fileSize];
		if (SDL_RWread(rw, buffer, fileSize, 1) == 1)
		{
			SDL_RWclose(rw);
			rapidjson::Document document;
			if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
			{
				LOG("Error loading previous scene. Scene file corrupted.");
				root = new GameObject("Scene");
			}
			else
			{				
				root = new GameObject("Scene");
				sprintf_s(root->gameObjectUUID, document["UUID"].GetString());
				rapidjson::Value children = document["children"].GetArray();
				bool isOk = true;
				for (rapidjson::Value::ValueIterator it = children.Begin(); it != children.End(); ++it)
				{
					GameObject* newGO = new GameObject("");	
					isOk = isOk && newGO->UnSerialize(*it, true);
				}
				if (!isOk)
				{
					CleanUp();
					root = new GameObject("Scene");
					LOG("Error loading scene");
				}
				else
				{
					root->transform->PropagateTransform();
					FlattenHierarchy(root);
				}
			}
			delete buffer;
		}
		else
		{
			LOG("Error loading previous scene %s", SDL_GetError());
			SDL_RWclose(rw);
		}
	}	
	return true;
}

update_status ModuleScene::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT
		|| App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN
		|| App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		Serialize();
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	LOG("Cleaning scene GameObjects.");
	for (std::vector<GameObject*>::reverse_iterator it = importedGameObjects.rbegin(); it != importedGameObjects.rend(); ++it) //must be cleaned backwards because dependencies
	{
		std::vector<GameObject*>::iterator it2 = std::find(sceneGameObjects.begin(), sceneGameObjects.end(), *it);
		if (it2 == sceneGameObjects.end()) //if it isn't in the instatiated list clean it - else it should be cleanend after
			RELEASE(*it);
	}
	for (std::vector<GameObject*>::reverse_iterator it = sceneGameObjects.rbegin(); it != sceneGameObjects.rend(); ++it) //must be cleaned backwards because dependencies
	{
		RELEASE(*it);
	}
	
	sceneGameObjects.resize(0);
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
	bool node_open = ImGui::TreeNodeEx(gObj->name, flags);
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
				if (movedGO != nullptr)
				{
					movedGO->parent->children.remove(movedGO);

					movedGO->parent = gObj;
					LOG("Moved gameobject %s", gObj->gameObjectUUID);
					gObj->children.push_back(movedGO);
					movedGO->transform->NewAttachment();
				}
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

bool ModuleScene::MakeParentInstantiated(char parentUUID[40], GameObject * son)
{
	if (strcmp(root->gameObjectUUID, parentUUID) == 0)
	{
		root->InsertChild(son);
		return true;
	}
	for (std::vector<GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		if (strcmp((*it)->gameObjectUUID, parentUUID) == 0)
		{
			(*it)->InsertChild(son);
			return true;
		}
	}
	return false;
}

bool ModuleScene::MakeParentImported(char parentUUID[40], GameObject * son)
{
	if (strcmp(directory->gameObjectUUID, parentUUID) == 0)
	{
		directory->InsertChild(son);
		return true;
	}
	for (std::vector<GameObject*>::iterator it = importedGameObjects.begin(); it != importedGameObjects.end(); ++it)
	{
		if (strcmp((*it)->gameObjectUUID, parentUUID) == 0)
		{
			(*it)->InsertChild(son);
			return true;
		}
	}
	return false;
}

GameObject * ModuleScene::FindInstanceOrigin(char instance[40])
{
	for (std::vector<GameObject*>::iterator it = importedGameObjects.begin(); it != importedGameObjects.end(); ++it)
	{
		if (strcmp((*it)->gameObjectUUID, instance) == 0)
		{
			return (*it);
		}
	}
	return nullptr;  //broken link
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
	bool hasMeshes = false;
	for (std::list<Component*>::iterator it2 = go->components.begin(); it2 != go->components.end(); ++it2)
	{
		if ((*it2)->type == Component::ComponentTypes::MESH_COMPONENT)
		{
			ComponentMesh* mesh = (ComponentMesh*)(*it2);
			mesh->SendToGPU();
			hasMeshes = true;
		}
	}
	if (hasMeshes)
		App->renderer->insertRenderizable(go);
	for (std::list<GameObject*>::iterator it = go->children.begin(); it != go->children.end(); ++it)
	{
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
	//save scene
	SDL_RWops *rw = SDL_RWFromFile("scene.dsc", "w");
	if (rw == nullptr)
	{
		LOG("Couldn't save scene. %s", SDL_GetError());
	}
	else
	{
		if (SDL_RWwrite(rw, sb.GetString(), strlen(sb.GetString()), 1) == 1)
		{
			LOG("Scene saved.");
		}
		else
		{
			LOG("Couldn't save scene. %s", SDL_GetError());
		}
		SDL_RWclose(rw);
	}

	rapidjson::StringBuffer sb2;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer2(sb2);
	directory->Serialize(writer2);	

	//save imported assets
	rw = SDL_RWFromFile("assets.dsc", "w");
	if (rw == nullptr)
	{
		LOG("Couldn't save imported hierarchy. %s", SDL_GetError());
	}
	else
	{
		if (SDL_RWwrite(rw, sb2.GetString(), strlen(sb2.GetString()), 1) == 1)
		{
			LOG("Imported hierarchy saved.");
		}
		else
		{
			LOG("Couldn't save imported hierarchy. %s", SDL_GetError());
		}
		SDL_RWclose(rw);
	}

}
