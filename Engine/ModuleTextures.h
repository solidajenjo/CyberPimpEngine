#pragma once
#include<vector>
#include "Module.h"
#include "Globals.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	unsigned Load(const char* path);

	std::vector<unsigned> textures;
};