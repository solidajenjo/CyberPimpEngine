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
#include <stack>
#include "SDL/include/SDL_filesystem.h"

bool ModuleScene::Init()
{
	LOG("Init Scene module");
	

	SDL_RWops* rw = SDL_RWFromFile("scene.dsc", "r");
	if (rw == nullptr) //no previous scene found
	{
		root = new GameObject("Scene");	
		directory = new GameObject("Assets");
		sceneGameObjects[root->gameObjectUUID] = root;
		sceneGameObjects[directory->gameObjectUUID] = directory;
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
				directory = new GameObject("Assets");
				sceneGameObjects[root->gameObjectUUID] = root;
				sceneGameObjects[directory->gameObjectUUID] = directory;
			}
			else
			{				
				rapidjson::Value gameObjects = document.GetArray();				
				for (rapidjson::Value::ValueIterator it = gameObjects.Begin(); it != gameObjects.End(); ++it)
				{
					if ((*it).HasMember("scene"))
					{
						root = new GameObject("");
						root->UnSerialize((*it)["scene"]);	
						sceneGameObjects[root->gameObjectUUID] = root;
					}
					else if ((*it).HasMember("assets"))
					{
						directory = new GameObject("");
						directory->UnSerialize((*it)["assets"]);
						sceneGameObjects[directory->gameObjectUUID] = directory;
					}
					else
					{
						GameObject* newGO = new GameObject("");
						if (newGO->UnSerialize(*it))
							sceneGameObjects[newGO->gameObjectUUID] = newGO;
					}
				}
				
				LinkGameObjects();					
				root->transform->PropagateTransform();
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

	for (std::map<std::string, GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		RELEASE((*it).second);	
	}
	sceneGameObjects.clear();
	LOG("Cleaning scene GameObjects. Done");
	selected = nullptr;
	sceneCamera = nullptr;
	return true;
}

void ModuleScene::InsertGameObject(GameObject * newGO)
{
	assert(newGO != nullptr);
	sceneGameObjects[newGO->gameObjectUUID] = newGO;
}

void ModuleScene::ImportGameObject(GameObject * newGO) 
{
	assert(newGO != nullptr);		
	if (strlen(newGO->parentUUID) == 0) 
	{	
		directory->InsertChild(newGO);		
	}
	sceneGameObjects[newGO->gameObjectUUID] = newGO;
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
	ImGui::PushID(gObj);
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
				GameObject* movedGO = FindInstanceOrigin(movedId);
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


bool ModuleScene::IsRoot(const GameObject * go) const
{
	return go == root || go == directory;
}

void ModuleScene::AttachToRoot(GameObject * go)
{
	root->InsertChild(go);
}

void ModuleScene::AttachToAssets(GameObject * go)
{
	directory->InsertChild(go);
}

bool ModuleScene::MakeParent(const std::string &parentUUID, GameObject * son)
{
	GameObject* parent = FindInstanceOrigin(parentUUID);
	if (parent != nullptr)
	{
		parent->InsertChild(son);		
		return true;
	}
	return false;
}

GameObject* ModuleScene::FindInstanceOrigin(const std::string &instance)
{
	std::map<std::string, GameObject*>::iterator it = sceneGameObjects.find(instance);
	if (it != sceneGameObjects.end())
		return (*it).second;
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


void ModuleScene::Serialize()
{
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	writer.StartArray();

	//Keep the roots to rebuild hierarchies
	writer.StartObject(); 
	writer.String("scene");
	root->Serialize(writer);
	writer.EndObject();
	writer.StartObject();
	writer.String("assets");
	directory->Serialize(writer);
	writer.EndObject();

	//Serialize all the remaining gameobjects
	for (std::map<std::string, GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		if (!IsRoot((*it).second))
			(*it).second->Serialize(writer);
	}

	writer.EndArray();

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
	

}

void ModuleScene::LinkGameObjects()
{
	for (std::map<std::string, GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		MakeParent((*it).second->parentUUID, (*it).second);
	}
}
