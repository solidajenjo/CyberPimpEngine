#include "ModuleSpacePartitioning.h"
#include "Application.h"
#include "ModuleScene.h"
#include "debugdraw.h"


bool ModuleSpacePartitioning::Init()
{

	return true;
}

update_status ModuleSpacePartitioning::Update()
{	
	if (App->scene->selected != nullptr && App->scene->selected->isInstantiated)
	{
		std::vector<GameObject*> intersections;
		quadTree.GetIntersections(*App->scene->selected->aaBBGlobal, intersections);
		for each (GameObject* go in intersections)
		{
			if (go != App->scene->selected && go->aaBBGlobal->ContainsQTree(*App->scene->selected->aaBBGlobal))
				LOG("%s intersects with %s", App->scene->selected->name, go->name);
		}
	}
	return UPDATE_CONTINUE;
}

bool ModuleSpacePartitioning::CleanUp()
{

	return true;
}
