#ifndef _MODULE_TEXTURES_H
#define _MODULE_TEXTURES_H

#include <list>
#include <string>
#include "Module.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures();

	bool Init() override;
	bool CleanUp() override;

	unsigned Load(const std::string& path, bool useMipMaps = false);
	void UnLoad(unsigned texNum);

	unsigned whiteFallback = 0u, blackFallback = 0u;
//members

	std::list<unsigned> textures; 
};

#endif