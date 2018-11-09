#ifndef _MODULE_RENDER_H
#define _MODULE_RENDER_H

#include "Module.h"
#include "Globals.h"

class ModuleRender : public Module
{
public:

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	void RecalcFrameBufferTexture() const;

//members

	void* context; // Opaque typedef void* openGL handler

	unsigned int framebuffer = 0;
	unsigned int texColorBuffer = 0;
	unsigned int viewPortWidth = 0;
	unsigned int viewPortHeight = 0;
};

#endif