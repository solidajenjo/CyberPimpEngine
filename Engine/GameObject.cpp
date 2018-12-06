#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "SceneImporter.h"
#include "MaterialImporter.h"
#include <queue>

void GameObject::InsertComponent(Component * newComponent)
{
	newComponent->owner = this;
	if (newComponent->type == Component::ComponentTypes::MESH_COMPONENT)
	{
		ComponentMesh* mesh = (ComponentMesh*)newComponent;		
		if (aaBB != nullptr) //accumulate AABBs //TODO: create global AABB when multi-mesh gameobjects
		{
			AABB tempAABB;
			tempAABB.SetFrom(&mesh->meshVertices[0], mesh->nVertices);
			std::vector<float3> aaBBPoints;
			aaBBPoints.push_back(tempAABB.minPoint);
			aaBBPoints.push_back(tempAABB.maxPoint);
			aaBBPoints.push_back(aaBB->minPoint);
			aaBBPoints.push_back(aaBB->maxPoint);
			aaBB->MinimalEnclosingAABB(&aaBBPoints[0], 4);
		}
		else
		{
			aaBB = new AABB();
			if (mesh->nVertices > 0)
				aaBB->SetFrom(&mesh->meshVertices[0], mesh->nVertices);
			else
				aaBB->SetFromCenterAndSize(newComponent->owner->transform->getGlobalPosition(), float3(.1f, .1f, .1f));
		}
	}
	components.push_back(newComponent);
}

void GameObject::InsertChild(GameObject * child)
{
	children.push_back(child);
	child->parent = this;
	sprintf_s(child->parentUUID, this->gameObjectUUID);
}

void GameObject::SetInstanceOf(char instanceOrigin[40])
{
	sprintf_s(instanceOf, instanceOrigin);
}

void GameObject::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("UUID"); writer.String(gameObjectUUID);
	writer.String("name"); writer.String(name);
	if (transform != nullptr)
	{
		writer.String("transform");
		transform->Serialize(writer);
	}
	writer.String("instanceOf"); writer.String(instanceOf);
	writer.String("path"); writer.String(path);
	writer.String("gameObjectType"); writer.Int((int)gameObjectType);
	switch (gameObjectType)
	{
		case GameObjectType::MATERIAL_CONTAINER:
		{
			MaterialImporter mi;
			mi.Save(path, (ComponentMaterial*) components.front()); //persist changes
			break; 
		}
	}
	writer.String("parentUUID"); writer.String(parentUUID);

	
	writer.String("components");
	writer.StartArray();
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Serialize(writer);
	}
	writer.EndArray();
	writer.EndObject();
}

bool GameObject::UnSerialize(rapidjson::Value &value, bool isInstantiated)
{
	bool isOk = true;
	sprintf_s(gameObjectUUID, value["UUID"].GetString());
	sprintf_s(name, value["name"].GetString());
	sprintf_s(parentUUID, value["parentUUID"].GetString());
	
	if (value.HasMember("transform"))
		transform->UnSerialize(value["transform"]);
	sprintf_s(parentUUID, value["parentUUID"].GetString());
	sprintf_s(instanceOf, value["instanceOf"].GetString());
	sprintf_s(path, value["path"].GetString());
	gameObjectType = (GameObjectType) value["gameObjectType"].GetInt();
	rapidjson::Value componentValues = value["components"].GetArray();
	bool hasMeshes = false;
	for (rapidjson::Value::ValueIterator it = componentValues.Begin(); it != componentValues.End(); ++it)
	{
		/*
		unsigned componentType = (*it)["type"].GetInt();		
		switch (componentType)
		{
			case (unsigned)Component::ComponentTypes::CAMERA_COMPONENT:
			{
				ComponentCamera* newCamera = new ComponentCamera();
				newCamera->UnSerialize(*it);
				InsertComponent(newCamera);
				break;
			}
			case (unsigned)Component::ComponentTypes::MESH_COMPONENT:
			{				
				if (isInstantiated)
				{
					char meshInstanceOf[40];
					sprintf_s(meshInstanceOf, (*it)["meshUUID"].GetString());
					if (strlen(meshInstanceOf) > 0)
					{
						GameObject* origin = App->scene->FindInstanceOrigin(instanceOf);
						if (origin == nullptr)
						{
							LOG("Broken link");
						}
						else
						{
							ComponentMesh* mesh = origin->GetMeshInstanceOrigin(meshInstanceOf);
							if (mesh == nullptr)
							{
								LOG("Broken link");
							}
							else
							{
								InsertComponent(mesh);
								mesh->SendToGPU();
								hasMeshes = true;								
							}
						}
					}
					else
					{
						ComponentMesh* newMesh = new ComponentMesh();
						newMesh->UnSerialize(*it);
						InsertComponent(newMesh);
						newMesh->SendToGPU(); //TODO send only once to gpu
						hasMeshes = true;
					}
				}
				break;
			}
			case (unsigned)Component::ComponentTypes::MATERIAL_COMPONENT:
			{
				MaterialImporter mi;				
				GameObject* mat = mi.Load(path);
				InsertComponent(mat->components.front());
				break;
			}
		}*/
		if (hasMeshes)
			App->renderer->insertRenderizable(this);
	}
	return isOk;
}

GameObject* GameObject::MakeInstanceOf() const
{
	GameObject* clonedGO = new GameObject(name);		
	sprintf_s(clonedGO->instanceOf, gameObjectUUID);
	clonedGO->transform->position = transform->position;
	clonedGO->transform->rotation = transform->rotation;
	clonedGO->transform->scale = transform->scale;
	clonedGO->transform->RecalcModelMatrix();
	for (std::list<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		clonedGO->components.push_back(*it);
	}
	for (std::list<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		clonedGO->InsertChild((*it)->MakeInstanceOf());
	}
	return clonedGO;
}

ComponentMesh* GameObject::GetMeshInstanceOrigin(char meshUUID[40]) const
{
	for (std::list<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->type == Component::ComponentTypes::MESH_COMPONENT)
		{
			ComponentMesh* mesh = (ComponentMesh*)(*it);
			if (strcmp(mesh->meshUUID, meshUUID) == 0)
			{
				return mesh;
			}
		}
	}
	return nullptr; //broken link
}
