#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleSpacePartitioning.h"
#include "SceneImporter.h"
#include "MaterialImporter.h"
#include <queue>



GameObject::GameObject(const char name[40])
{
	transform = new Transform(this); //notice transform who owns it
	xg::Guid guid = xg::newGuid();
	std::string uuid = guid.str();
	sprintf_s(gameObjectUUID, uuid.c_str());
	sprintf_s(this->name, name);
	aaBB = new AABB();
	aaBBGlobal = new AABB();
	aaBB->SetFromCenterAndSize(transform->position, float3(.1f, .1f, .1f));
}

GameObject::GameObject(char UUID[40], Transform* transform) : transform(transform)
{
	sprintf_s(gameObjectUUID, UUID);
	aaBB = new AABB();
	aaBBGlobal = new AABB();
	aaBB->SetFromCenterAndSize(transform->position, float3(.1f, .1f, .1f));
};

GameObject::~GameObject()
{
	if (isRenderizable)
		App->renderer->removeRenderizable(this);

	RELEASE(transform); 
	RELEASE(aaBB);
	RELEASE(aaBBGlobal);
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->Release())
			RELEASE(*it);
	}
	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{		
		App->scene->DestroyGameObject(*it);
		RELEASE(*it);
	}
}


void GameObject::InsertComponent(Component * newComponent)
{
	newComponent->owner = this;
	if (newComponent->type == Component::ComponentTypes::MESH_COMPONENT)
	{
		ComponentMesh* mesh = (ComponentMesh*)newComponent;		
		if (aaBB != nullptr)
		{
			float3* corners = new float3[16];
			AABB tempAABB;
			tempAABB.SetFrom(&mesh->meshVertices[0], mesh->nVertices);

			aaBB->GetCornerPoints(&corners[0]);
			tempAABB.GetCornerPoints(&corners[8]);
			aaBB->Enclose(&corners[0], 16);		
		}
		else
		{
			RELEASE(aaBB);
			aaBB = new AABB();
			RELEASE(aaBBGlobal);
			aaBBGlobal = new AABB();
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
	writer.String("isInstantiated"); writer.Bool(isInstantiated);
	writer.String("isStatic"); writer.Bool(isStatic);
	writer.String("active"); writer.Bool(active);
	if (transform != nullptr)
	{
		writer.String("transform");
		transform->Serialize(writer);
	}
	writer.String("instanceOf"); writer.String(instanceOf);
	writer.String("path"); writer.String(path);
	
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

bool GameObject::UnSerialize(rapidjson::Value &value)
{
	sprintf_s(gameObjectUUID, value["UUID"].GetString());
	sprintf_s(name, value["name"].GetString());
	sprintf_s(parentUUID, value["parentUUID"].GetString());
	isInstantiated = value["isInstantiated"].GetBool();
	isStatic = value["isStatic"].GetBool();
	active = value["active"].GetBool();
	if (value.HasMember("transform"))
		transform->UnSerialize(value["transform"]);
	else
		RELEASE(transform);
	sprintf_s(parentUUID, value["parentUUID"].GetString());
	sprintf_s(instanceOf, value["instanceOf"].GetString());
	sprintf_s(path, value["path"].GetString());
	rapidjson::Value componentValues = value["components"].GetArray();

	for (rapidjson::Value::ValueIterator it = componentValues.Begin(); it != componentValues.End(); ++it)
	{
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
					
			case (unsigned)Component::ComponentTypes::MATERIAL_COMPONENT:
			{
				char materialPath[1024];
				sprintf_s(materialPath, (*it)["materialPath"].GetString());
				ComponentMaterial* mat = ComponentMaterial::GetMaterial(materialPath);
				if (mat != nullptr)
					InsertComponent(mat);
				break;
			}

			case (unsigned)Component::ComponentTypes::MESH_COMPONENT:
			{
				if ((*it)["primitive"].GetInt() == (int)ComponentMesh::Primitives::VOID_PRIMITIVE) //is a mesh
				{
					char meshPath[1024];
					sprintf_s(meshPath, (*it)["meshPath"].GetString());
					ComponentMesh* mesh = ComponentMesh::GetMesh(meshPath);
					if (mesh != nullptr)
					{
						InsertComponent(mesh);
						if (isInstantiated)
						{
							mesh->SendToGPU();
							App->renderer->insertRenderizable(this);
						}
					}
				}
				else
				{
					ComponentMesh* newMesh = new ComponentMesh();
					newMesh->UnSerialize(*it);
					newMesh->SendToGPU();
					InsertComponent(newMesh);
					App->renderer->insertRenderizable(this);
				}
			}
		}

	}		

	return true;
}

void GameObject::PropagateStaticCheck()
{
	std::queue<GameObject*> Q;
	GameObject* origin = this;
	while (!App->scene->IsRoot(origin->parent)) //crawl up until find a root descendant
		origin = origin->parent;

	Q.push(origin);
	while (!Q.empty()) //propagate
	{
		origin = Q.front(); Q.pop();
		origin->isStatic = isStatic;
		for (std::list<GameObject*>::iterator it = origin->children.begin(); it != origin->children.end(); ++it)
		{
			Q.push(*it);			
		}
	}
	App->spacePartitioning->quadTree.Calculate();
}


GameObject* GameObject::MakeInstanceOf() const
{
	GameObject* clonedGO = new GameObject(name);		
	sprintf_s(clonedGO->instanceOf, gameObjectUUID);
	clonedGO->transform->position = transform->position;
	clonedGO->transform->rotation = transform->rotation;
	clonedGO->transform->scale = transform->scale;
	clonedGO->transform->RecalcModelMatrix();
	clonedGO->isInstantiated = true;
	for (std::list<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->type == Component::ComponentTypes::MESH_COMPONENT)
		{
			ComponentMesh* mesh = (ComponentMesh*)(*it);
			ComponentMesh* newMesh = ComponentMesh::GetMesh(mesh->meshPath); //get an instance
			newMesh->SendToGPU();
			App->renderer->insertRenderizable(clonedGO);
			clonedGO->InsertComponent(newMesh);
		}
		else
			clonedGO->InsertComponent(*it);
	}
	for (std::list<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		clonedGO->InsertChild((*it)->MakeInstanceOf());
	}
	App->scene->InsertGameObject(clonedGO);
	return clonedGO;
}

GameObject * GameObject::Clone() const
{
	GameObject* clonedGO = new GameObject(name);
	sprintf_s(clonedGO->instanceOf, instanceOf);
	if (transform != nullptr)
	{
		clonedGO->transform = transform->Clone();
		clonedGO->transform->owner = clonedGO;
		clonedGO->transform->RecalcModelMatrix();
	}
	clonedGO->isInstantiated = isInstantiated;
	for (std::list<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->type == Component::ComponentTypes::MESH_COMPONENT)
		{
			ComponentMesh* mesh = (ComponentMesh*)(*it);
			ComponentMesh* newMesh;
			if (mesh->primitiveType == ComponentMesh::Primitives::VOID_PRIMITIVE)
				newMesh = ComponentMesh::GetMesh(mesh->meshPath); //get an instance
			else
			{
				newMesh = mesh->Clone();				
			}
			newMesh->SendToGPU();
			App->renderer->insertRenderizable(clonedGO);
			clonedGO->InsertComponent(newMesh);
		}
		else
			clonedGO->InsertComponent((*it)->Clone());
	}
	for (std::list<GameObject*>::const_iterator it = children.begin(); it != children.end(); ++it)
	{
		clonedGO->InsertChild((*it)->Clone());
	}
	App->scene->InsertGameObject(clonedGO);
	return clonedGO;
}
