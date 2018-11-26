#include "GameObject.h"
#include "ComponentMesh.h"

void GameObject::InsertComponent(Component * newComponent)
{
	newComponent->owner = this;
	if (newComponent->type == "Mesh")
	{
		ComponentMesh* mesh = (ComponentMesh*)newComponent;
		aaBB.SetNegativeInfinity();
		aaBB.Enclose(&mesh->meshVertices[0], mesh->nVertices);
	}
	components.push_back(newComponent);
}
