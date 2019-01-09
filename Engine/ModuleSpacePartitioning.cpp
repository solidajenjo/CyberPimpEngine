#include "ModuleSpacePartitioning.h"
#include "Application.h"
#include "ModuleScene.h"
#include "debugdraw.h"


bool ModuleSpacePartitioning::Init() //TODO:Clear Trees on new
{
	kDTree.Init();
	aabbTree.Init();
	return true;
}

update_status ModuleSpacePartitioning::Update()
{		
	return UPDATE_CONTINUE;
}

bool ModuleSpacePartitioning::CleanUp()
{
	aabbTree.CleanUp();
	return true;
}


