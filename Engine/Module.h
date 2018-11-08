#ifndef _MODULE_H_
#define _MODULE_H_

#include "Globals.h"

class Application;

class Module
{
public:

	Module()
	{
	}

	virtual ~Module() {}; //force call to descendant destructor to avoid partial destruction

	virtual bool Init() 
	{
		return true; 
	}

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};

#endif