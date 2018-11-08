#ifndef _MODULE_RENDER_H
#define _MODULE_RENDER_H

#include "Module.h"
#include "Globals.h"

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void RecalcFrameBufferTexture() const;

public:
	void* context; //TODO: <-- ?

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int viewPortWidth;
	unsigned int viewPortHeight;
};

#endif