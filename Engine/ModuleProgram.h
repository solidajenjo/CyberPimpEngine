#ifndef _MODULE_PROGRAM_H
#define _MODULE_PROGRAM_H

#include "Module.h"
#include <string>

class ModuleProgram :
	public Module
{
public:

	ModuleProgram(const std::string& vsName, const std::string& fsName) : vsName(vsName), fsName(fsName) {};	

	bool Init() override;
	void UseProgram() const;
	void StopUseProgram() const;
	bool CleanUp() override;
	
//members

	unsigned program = 0;
	std::string vsName = "", fsName = "";

	//TODO: multi program funcionality
private:

	char* readFile(const std::string& name);  //Should be cleaned by the caller
	
};

#endif
