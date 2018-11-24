#ifndef _MODULE_FRAMEBUFFER_H
#define _MODULE_FRAMEBUFFER_H

#include "Module.h"
#include "Globals.h"

class ComponentMesh;

class ModuleFrameBuffer : public Module
{
public:

	bool Init() override;
	update_status PreUpdate() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	bool RecalcFrameBufferTexture();

//members

	unsigned int framebuffer = 0;
	unsigned int texColorBuffer = 0;
	unsigned int viewPortWidth = 0;
	unsigned int viewPortHeight = 0;
	unsigned int colorRenderbuffer = 0;
	unsigned int depthRenderbuffer = 0;

	bool frustumCulling = true;
	ComponentMesh* skyBox = nullptr;
};

#endif