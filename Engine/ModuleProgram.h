#ifndef _MODULE_PROGRAM_H
#define _MODULE_PROGRAM_H

#include "Module.h"
#include <string>

class ModuleProgram :
	public Module
{
public:

	enum class Shaders
	{
		DEFAULT,
		FORWARD_RENDERING,
		NORMAL_INSPECTOR
	};

	bool Init() override;

	bool Compile(Shaders shaderType, std::string vsName, std::string fsName);
	void StopUseProgram() const;
	bool CleanUp() override;
	
//members

	unsigned* default = nullptr;
	unsigned* directRenderingProgram = nullptr;
	unsigned* normalInspectorProgram = nullptr;

private:

	char* readFile(const std::string& name);  //Should be cleaned by the caller
	
};

#endif
