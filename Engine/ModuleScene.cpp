#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleSpacePartitioning.h"
#include "QuadTree.h"
#include "ModuleInput.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ComponentMesh.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include "SDL/include/SDL_rwops.h"
#include <stack>
#include "SDL/include/SDL_filesystem.h"

#define HIERARCHY_DRAW_TAB 10

bool ModuleScene::Init()
{
	LOG("Init Scene module");
	
	bool staticGameObjects = false;
	bool nonStaticGameObjects = false;

	SDL_RWops* rw = SDL_RWFromFile("scene.dsc", "r");
	if (rw == nullptr) //no previous scene found
	{
		root = new GameObject("Scene", true);
		directory = new GameObject("Assets", true);
		modelFolder = new GameObject("Models", true);
		mapFolder = new GameObject("Maps", true);
		materialFolder = new GameObject("Materials", true);
		AttachToAssets(modelFolder);
		AttachToAssets(mapFolder);
		AttachToAssets(materialFolder);
		sceneGameObjects[root->gameObjectUUID] = root;
		sceneGameObjects[directory->gameObjectUUID] = directory;
		sceneGameObjects[modelFolder->gameObjectUUID] = modelFolder;
		sceneGameObjects[mapFolder->gameObjectUUID] = mapFolder;
		sceneGameObjects[materialFolder->gameObjectUUID] = materialFolder;
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
				root = new GameObject("Scene", true);
				directory = new GameObject("Assets", true);
				modelFolder = new GameObject("Models", true);
				mapFolder = new GameObject("Maps", true);
				materialFolder = new GameObject("Materials", true);
				AttachToAssets(modelFolder);
				AttachToAssets(mapFolder);
				AttachToAssets(materialFolder);
				sceneGameObjects[root->gameObjectUUID] = root;
				sceneGameObjects[root->gameObjectUUID] = root;
				sceneGameObjects[directory->gameObjectUUID] = directory;
				sceneGameObjects[modelFolder->gameObjectUUID] = modelFolder;
				sceneGameObjects[mapFolder->gameObjectUUID] = mapFolder;
				sceneGameObjects[materialFolder->gameObjectUUID] = materialFolder;
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
					else if ((*it).HasMember("models"))
					{
						modelFolder = new GameObject("");
						modelFolder->UnSerialize((*it)["models"]);
						sceneGameObjects[modelFolder->gameObjectUUID] = modelFolder;
					}
					else if ((*it).HasMember("maps"))
					{
						mapFolder = new GameObject("");
						mapFolder->UnSerialize((*it)["maps"]);
						sceneGameObjects[mapFolder->gameObjectUUID] = mapFolder;
					}
					else if ((*it).HasMember("materials"))
					{
						materialFolder = new GameObject("");
						materialFolder->UnSerialize((*it)["materials"]);
						sceneGameObjects[materialFolder->gameObjectUUID] = materialFolder;
					}
					else
					{
						GameObject* newGO = new GameObject("");						
						if (newGO->UnSerialize(*it))
						{
							sceneGameObjects[newGO->gameObjectUUID] = newGO;
							if (newGO->isStatic)
								staticGameObjects = true;
							else
								nonStaticGameObjects = true;
						}
					}
				}
				
				LinkGameObjects();					
				if (staticGameObjects)
				{
					App->spacePartitioning->quadTree.Calculate();
				}
				if (nonStaticGameObjects)
				{
					App->spacePartitioning->kDTree.Calculate();					
				}
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

	RELEASE(root);
	RELEASE(directory);
	
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

void ModuleScene::ImportGameObject(GameObject * newGO, ImportedType type)
{
	assert(newGO != nullptr);		
	switch (type)
	{
	case ImportedType::MAP:
		AttachToMaps(newGO);
		break;
	case ImportedType::MODEL:
		AttachToModels(newGO);
		break;
	case ImportedType::MATERIAL:
		AttachToMaterials(newGO);
		break;
	}		
		
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
		
	std::stack<GameObject*> S;
	std::stack<int> depthStack;	
	S.push(gObj);	
	depthStack.push(0);	
	while (!S.empty())
	{
		gObj = S.top(); S.pop();		
		ImGui::PushID(gObj->gameObjectUUID);
		unsigned depth = depthStack.top(); depthStack.pop();
		if (gObj->children.size() == 0)
			flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
		else
			flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (gObj->selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;

		}

		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x + depth, pos.y));
		if (ImGui::TreeNodeEx(gObj->name, flags))
		{			
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
							LOG("Moved gameobject %s", movedGO->gameObjectUUID);
							gObj->InsertChild(movedGO);
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

			if (gObj->children.size() > 0)
			{
				for (std::list<GameObject*>::iterator it = gObj->children.begin(); it != gObj->children.end(); ++it)
				{
					S.push(*it);	
					depthStack.push(depth + HIERARCHY_DRAW_TAB);
				}
			}
			ImGui::TreePop();			

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

		if (!IsRoot(gObj) && gObj == selected && ImGui::BeginPopupContextItem("GO_CONTEXT"))
		{
			ImGui::Text("GameObject operations");
			ImGui::Separator();			
			bool deleteGameObject = false;						
			ImVec2 curPos = ImGui::GetCursorScreenPos();
			ImGui::Text("Delete    - Ctrl + X");
			ImVec2 size = ImGui::GetItemRectSize();
			if (ImGui::IsItemHovered())
			{				
				ImGui::GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + size.x * 1.1f, curPos.y + size.y), IM_COL32(200, 200, 200, 55));
			}
			if (ImGui::IsItemClicked())
				deleteGameObject = true;
			

			if (deleteGameObject)
			{
				ImGui::OpenPopup("Confirm");
			}

			bool closeContextPopup = false;
			if (ImGui::BeginPopup("Confirm", ImGuiWindowFlags_Modal))
			{
				ImGui::Text("Are you sure?");
				if (ImGui::Button("NO", ImVec2(100, 20)))
				{
					ImGui::CloseCurrentPopup();
					closeContextPopup = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("YES", ImVec2(100, 20)))
				{
					DeleteGameObject(gObj);
					selected = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			curPos = ImGui::GetCursorScreenPos();
			ImGui::Text("Duplicate - Ctrl + D");
			size = ImGui::GetItemRectSize();
			if (ImGui::IsItemHovered())
			{
				ImGui::GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + size.x * 1.1f, curPos.y + size.y), IM_COL32(200, 200, 200, 55));
			}
			if (ImGui::IsItemClicked())
			{
				gObj->parent->InsertChild(gObj->Clone());
				gObj->parent->transform->PropagateTransform();
				closeContextPopup = true;
			}

			if (closeContextPopup)
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}	

}


bool ModuleScene::IsRoot(const GameObject * go) const
{
	return go == root || go == directory || go == mapFolder || go == modelFolder || go == materialFolder;
}

void ModuleScene::AttachToRoot(GameObject * go)
{
	assert(go != nullptr);
	root->InsertChild(go);
	FlattenHierarchyOnImport(go);
}

void ModuleScene::AttachToAssets(GameObject * go)
{
	assert(go != nullptr);
	directory->InsertChild(go);
	FlattenHierarchyOnImport(go);
}

void ModuleScene::AttachToMaps(GameObject * go)
{
	assert(go != nullptr);
	mapFolder->InsertChild(go);
	FlattenHierarchyOnImport(go);
}

void ModuleScene::AttachToModels(GameObject * go)
{
	assert(go != nullptr);
	modelFolder->InsertChild(go);
	FlattenHierarchyOnImport(go);
}

void ModuleScene::AttachToMaterials(GameObject * go)
{
	assert(go != nullptr);
	materialFolder->InsertChild(go);
	FlattenHierarchyOnImport(go);
}

void ModuleScene::DestroyGameObject(GameObject * destroyableGO)
{
	assert(destroyableGO != nullptr);
	destroyableGO->parent->children.remove(destroyableGO);
	sceneGameObjects.erase(destroyableGO->gameObjectUUID);
}

void ModuleScene::DeleteGameObject(GameObject* go, bool isAsset)
{
	assert(go != nullptr);
	go->parent->children.remove(go);
	sceneGameObjects.erase(go->gameObjectUUID);
	std::stack<GameObject*> S;
	std::stack<GameObject*> DFS; // start the destroy from the leaves of the tree and upwards to avoid crashes
	S.push(go);
	while (!S.empty())
	{
		GameObject* node = S.top();
		S.pop();
		for (std::list<GameObject*>::iterator it = node->children.begin(); it != node->children.end(); ++it)
		{
			S.push(*it);
			DFS.push(*it);
		}
	}

	while (!DFS.empty())
	{
		DestroyGameObject(DFS.top());
		RELEASE(DFS.top());
		DFS.pop();
	}
	RELEASE(go); 	
}

bool ModuleScene::MakeParent(const std::string &parentUUID, GameObject * son)
{
	GameObject* parent = FindInstanceOrigin(parentUUID);
	if (parent != nullptr)
	{
		parent->InsertChild(son);
		if (parent->transform != nullptr)
			parent->transform->PropagateTransform();
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

void ModuleScene::GetStaticGlobalAABB(AABB* globalAABB, std::vector<GameObject*> &staticGOs) const
{
	bool first = true;
	for (std::map<std::string, GameObject*>::const_iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		float3* corners = new float3[16];
		if ((*it).second->isInstantiated && (*it).second->isStatic && (*it).second->aaBBGlobal != nullptr)
		{	
			if (first)
			{				
				globalAABB->SetNegativeInfinity();
				globalAABB->Enclose(*(*it).second->aaBBGlobal);
				first = false;
			}
			else
			{
				(*it).second->aaBBGlobal->GetCornerPoints(&corners[0]);
				globalAABB->GetCornerPoints(&corners[8]);
				globalAABB->Enclose(&corners[0], 16);
			}
			staticGOs.push_back((*it).second);
		}
		RELEASE(corners);
	}
}

void ModuleScene::GetNonStaticGlobalAABB(AABB * globalAABB, std::vector<GameObject*>& nonStaticGOs) const
{
	bool first = true;
	for (std::map<std::string, GameObject*>::const_iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		float3* corners = new float3[16];
		if ((*it).second->isInstantiated && !(*it).second->isStatic && (*it).second->aaBBGlobal != nullptr)
		{
			if (first)
			{
				globalAABB->SetNegativeInfinity();
				globalAABB->Enclose(*(*it).second->aaBBGlobal);
				first = false;
			}
			else
			{
				(*it).second->aaBBGlobal->GetCornerPoints(&corners[0]);
				globalAABB->GetCornerPoints(&corners[8]);
				globalAABB->Enclose(&corners[0], 16);
			}
			nonStaticGOs.push_back((*it).second);
		}
		RELEASE(corners);
	}
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

	//serialize folders
	writer.StartObject();
	writer.String("maps");	
	mapFolder->Serialize(writer);
	writer.EndObject();
	writer.StartObject();
	writer.String("materials");
	materialFolder->Serialize(writer);
	writer.EndObject();
	writer.StartObject();
	writer.String("models");
	modelFolder->Serialize(writer);
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

void ModuleScene::FlattenHierarchyOnImport(GameObject * go)
{
	assert(go != nullptr);
	std::stack<GameObject*> S;
	S.push(go);

	GameObject* node;

	while (!S.empty())
	{
		node = S.top();
		S.pop();
		sceneGameObjects[node->gameObjectUUID] = node;

		for (std::list<GameObject*>::iterator it = node->children.begin(); it != node->children.end(); ++it)
			S.push(*it);
	}
}
