#ifndef _MODULE_TEXTURES_H
#define _MODULE_TEXTURES_H

#include <vector>
#include <string>
#include "Module.h"
#include "Globals.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool CleanUp() override;

	unsigned Load(std::string path);

//members

	std::vector<unsigned> textures; //TODO: Store texture info
};

#endif