#ifndef _MODULE_TEXTURES_H
#define _MODULE_TEXTURES_H

#include "Module.h"
#include "Globals.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

};

#endif