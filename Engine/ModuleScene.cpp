#include "GameObject.h"
#include "ModuleScene.h"
#include "ModuleFrameBuffer.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleEditorCamera.h"
#include "ModuleSpacePartitioning.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "SubModuleEditorGameViewPort.h"
#include "SubModuleEditorToolBar.h"
#include "QuadTree.h"
#include "ModuleInput.h"
#include "Application.h"
#include "imgui/imgui.h"
#include "ComponentMesh.h"
#include "ComponentMap.h"
#include "ComponentCamera.h";
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"
#include "SDL/include/SDL_rwops.h"
#include <stack>
#include "SDL/include/SDL_filesystem.h"

#define HIERARCHY_DRAW_TAB 10

bool ModuleScene::Init()
{
	LOG("Init Scene module");
	root = new GameObject("Scene", true);
	directory = new GameObject("Assets", true);
	modelFolder = new GameObject("Models", true);
	mapFolder = new GameObject("Maps", true);
	materialFolder = new GameObject("Materials", true);
	App->spacePartitioning->kDTree.Init();
	AttachToAssets(modelFolder);
	AttachToAssets(mapFolder);
	AttachToAssets(materialFolder);
	return true;
}

update_status ModuleScene::Update()
{
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
	App->textures->CleanUp();
	return true;
}

bool ModuleScene::SaveScene(const std::string & path) const
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
	for (std::map<std::string, GameObject*>::const_iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		if (!IsRoot((*it).second))
			(*it).second->Serialize(writer);
	}

	writer.EndArray();

	if (App->fileSystem->Write(path, sb.GetString(), strlen(sb.GetString()), true))
	{
		LOG("Scene saved.");
	}

	rapidjson::StringBuffer sbConfig;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writerConfig(sbConfig);

	writerConfig.StartObject();
	writerConfig.String("scale"); writerConfig.Double(App->appScale);
	writerConfig.String("kdTreeDepth"); writerConfig.Int(App->spacePartitioning->kDTree.maxDepth);
	writerConfig.String("kdTreeBucketSize"); writerConfig.Int(App->spacePartitioning->kDTree.bucketSize);
	writerConfig.String("quadTreeDepth"); writerConfig.Int(App->spacePartitioning->quadTree.maxDepth);
	writerConfig.String("quadTreeBucketSize"); writerConfig.Int(App->spacePartitioning->quadTree.bucketSize);
	writerConfig.String("aa"); writerConfig.Int((int)App->editor->gameViewPort->antialiasing);
	writerConfig.String("editorCamera");
	App->camera->editorCamera.Serialize(writerConfig);
	writerConfig.EndObject();

	if (App->fileSystem->Write(path + ".cfg", sbConfig.GetString(), strlen(sbConfig.GetString()), true))
	{
		LOG("Scene configuration saved.");
	}
	return true;
}

bool ModuleScene::LoadScene(const std::string & path) 
{
	LOG("Loading scene %s", path.c_str());
	CleanUp();
	bool staticGameObjects = false;
	bool nonStaticGameObjects = false;
	if (!App->fileSystem->Exists(path + ".cfg"))
	{
		LOG("Error loading scene configuration %s", (path + ".cfg").c_str());
	}
	else
	{
		unsigned fileSize = App->fileSystem->Size(path + ".cfg");
		char* buffer = new char[fileSize];
		if (App->fileSystem->Read(path + ".cfg", buffer, fileSize))
		{
			rapidjson::Document document;
			if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
			{
				LOG("Error loading scene %s. Scene file corrupted.", (path + ".cfg").c_str());
			}
			else
			{
				rapidjson::Value config = document.GetObjectA();
				App->appScale = config["scale"].GetDouble();
				switch ((int)App->appScale)
				{
				case 1:
					App->editor->toolBar->scale_item_current = App->editor->toolBar->scale_items[0];
					break;
				case 10:
					App->editor->toolBar->scale_item_current = App->editor->toolBar->scale_items[1];
					break;
				case 100:
					App->editor->toolBar->scale_item_current = App->editor->toolBar->scale_items[2];
					break;
				case 1000:
					App->editor->toolBar->scale_item_current = App->editor->toolBar->scale_items[3];
					break;
				}
				App->spacePartitioning->kDTree.maxDepth = config["kdTreeDepth"].GetInt();
				App->spacePartitioning->kDTree.bucketSize = config["kdTreeBucketSize"].GetInt();
				App->spacePartitioning->quadTree.maxDepth = config["quadTreeDepth"].GetInt();
				App->spacePartitioning->quadTree.bucketSize = config["quadTreeBucketSize"].GetInt();
				App->editor->gameViewPort->antialiasing = (SubModuleEditorGameViewPort::AntiaAliasing)config["aa"].GetInt();
				App->editor->toolBar->aa_item_current = App->editor->toolBar->aa_items[(unsigned)App->editor->gameViewPort->antialiasing];
				rapidjson::Value serializedCam = config["editorCamera"].GetObjectA();
				App->camera->editorCamera.UnSerialize(serializedCam);
			}
		}
	}
	if (!App->fileSystem->Exists(path)) //no previous scene found
	{
		Init();
		sceneGameObjects[root->gameObjectUUID] = root;
		sceneGameObjects[directory->gameObjectUUID] = directory;
		sceneGameObjects[modelFolder->gameObjectUUID] = modelFolder;
		sceneGameObjects[mapFolder->gameObjectUUID] = mapFolder;
		sceneGameObjects[materialFolder->gameObjectUUID] = materialFolder;
		LOG("Error loading scene %s. Not found", path.c_str());
	}
	else
	{
		//restore instantiated gameobjects
		unsigned fileSize = App->fileSystem->Size(path);
		char* buffer = new char[fileSize];
		if (App->fileSystem->Read(path, buffer, fileSize))
		{
			rapidjson::Document document;
			if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
			{
				LOG("Error loading scene %s. Scene file corrupted.", path.c_str());
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
					App->spacePartitioning->kDTree.Init();
					App->spacePartitioning->kDTree.Calculate();
				}
				root->transform->PropagateTransform();
			}
			delete buffer;
		}
		else
		{
			LOG("Error loading scene %s",  path.c_str());			
		}
	}
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
	std::stack<ImVec2> parentPosition;
	S.push(gObj);	
	depthStack.push(0);	
	parentPosition.push(ImGui::GetCursorPos());
	while (!S.empty())
	{
		gObj = S.top(); S.pop();		
		ImGui::PushID(gObj->gameObjectUUID);
		unsigned depth = depthStack.top(); depthStack.pop();
		ImVec2 parentPos = parentPosition.top(); parentPosition.pop();
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
		pos = ImGui::GetCursorScreenPos();
		if (!IsRoot(gObj))
		{
			ImGui::GetWindowDrawList()->AddLine(ImVec2(pos.x, pos.y + 5.f), ImVec2(pos.x, parentPos.y), ImColor(1.f, 1.f, 1.f, 1.f));
			ImGui::GetWindowDrawList()->AddLine(ImVec2(pos.x, pos.y + 5.f), ImVec2(pos.x + 5.f, pos.y + 5.f), ImColor(1.f, 1.f, 1.f, 1.f));
		}
		if (ImGui::TreeNodeEx(gObj->name, flags))
		{		
			
			if (gObj->children.size() > 0)
			{
				for (std::list<GameObject*>::iterator it = gObj->children.begin(); it != gObj->children.end(); ++it)
				{
					S.push(*it);	
					depthStack.push(depth + HIERARCHY_DRAW_TAB);
					parentPosition.push(ImGui::GetCursorScreenPos());
				}
			}
			ImGui::TreePop();			

		}
		if (isWorld)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("GAMEOBJECT_ID", &gObj->gameObjectUUID, sizeof(gObj->gameObjectUUID));
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_ID"))
				{
					char movedId[sizeof(gObj->gameObjectUUID)];
					sprintf_s(movedId, (char*)payload->Data); 
					GameObject* movedGO = FindInstanceOrigin(movedId);
					if (movedGO != nullptr)
					{
						movedGO->parent->children.remove(movedGO);

						movedGO->parent = gObj;
						LOG("Moved gameobject %s", movedGO->name);
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
				ImGui::SetDragDropPayload("IMPORTED_GAMEOBJECT_ID", &gObj->gameObjectUUID, sizeof(gObj->gameObjectUUID)); 
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
					App->spacePartitioning->kDTree.Calculate();
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
		if ((*it).second->layer == GameObject::GameObjectLayers::WORLD_VOLUME && (*it).second->isInstantiated && (*it).second->isStatic && (*it).second->aaBBGlobal != nullptr)
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

void ModuleScene::GetNonStaticGlobalAABB(AABB* globalAABB, std::vector<GameObject*>& nonStaticGOs, unsigned &GOCount) const
{
	bool first = true;
	for (std::map<std::string, GameObject*>::const_iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		float3* corners = new float3[16];
		if ((*it).second->isInstantiated && (*it).second->layer == GameObject::GameObjectLayers::WORLD_VOLUME && !(*it).second->isStatic && (*it).second->aaBBGlobal != nullptr)
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
			nonStaticGOs[++GOCount] = (*it).second;
			assert(GOCount < BUCKET_MAX);
		}
		RELEASE(corners);
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
