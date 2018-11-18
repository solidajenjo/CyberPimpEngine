#ifndef _MODULE_EDITOR_CAMERA_H
#define _MODULE_EDITOR_CAMERA_H

#include "Module.h"
#include "Globals.h"
#include "ComponentCamera.h"

class ModuleEditorCamera :
	public Module
{
public:

	bool Init() override;
	update_status Update() override;
	
	//members

	ComponentCamera editorCamera;

private:
	float focusLerp = 0.f;
	bool movementStart = false;

};

#endif;

