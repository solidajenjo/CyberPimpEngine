#ifndef _MODULE_TEXTURES_H
#define _MODULE_TEXTURES_H

#include <list>
#include <string>
#include "Module.h"
#include "Globals.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures();

	bool CleanUp() override;

	unsigned Load(const std::string& path);
	void UnLoad(unsigned texNum);

//members

	std::list<unsigned> textures; 
};

#endif