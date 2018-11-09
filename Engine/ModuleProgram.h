#ifndef _MODULE_PROGRAM_H
#define _MODULE_PROGRAM_H

#include "Module.h"
#include <string>

class ModuleProgram :
	public Module
{
public:
	ModuleProgram(std::string vsName, std::string fsName) : vsName(vsName), fsName(fsName) {};	

	bool Init() override;
	void useProgram();
	bool CleanUp() override;
	
//members

	unsigned program = 0;
	std::string vsName, fsName;

	//TODO: multi program funcionality
private:

	char* readFile(std::string name);  //Should be cleaned by the caller
	
};

#endif
