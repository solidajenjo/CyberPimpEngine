#pragma once
#include "Module.h"
#include "GL/glew.h"

class ModuleProgram :
	public Module
{
public:
	ModuleProgram(char* vsName, char* fsName);
	~ModuleProgram();

	bool Init();
	void useProgram();
	bool CleanUp();
	
	GLuint program;
	char* vsName;
	char* fsName;

private:

	char* readFile(char* name);
	
};

