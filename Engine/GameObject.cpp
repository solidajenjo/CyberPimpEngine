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
