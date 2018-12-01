#include "GameObject.h"
#include "ComponentMesh.h"

void GameObject::InsertComponent(Component * newComponent)
{
	newComponent->owner = this;
	if (strcmp(newComponent->type, "Mesh") == 0)
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
			aaBB->SetFrom(&mesh->meshVertices[0], mesh->nVertices);
		}
	}
	components.push_back(newComponent);
}

void GameObject::InsertChild(GameObject * child)
{
	children.push_back(child);
	child->parent = this;
}

void GameObject::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("UUID"); writer.String(gameObjectUUID);
	if (parent != nullptr)
	{
		writer.String("Parent"); writer.String(parent->gameObjectUUID);
	}		

	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
		(*it)->Serialize(writer);
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		(*it)->Serialize(writer);

	writer.EndObject();
}

void GameObject::Clone(const GameObject & clonedGO)
{

}
