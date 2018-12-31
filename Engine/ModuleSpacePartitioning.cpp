#include "ModuleSpacePartitioning.h"
#include "Application.h"
#include "ModuleScene.h"
#include "debugdraw.h"


bool ModuleSpacePartitioning::Init()
{
	kDTree.Init();
	return true;
}

update_status ModuleSpacePartitioning::Update()
{	
	kDTree.Calculate();
	return UPDATE_CONTINUE;
}

bool ModuleSpacePartitioning::CleanUp()
{

	return true;
}


