#ifndef _MODULE_FRAMEBUFFER_H
#define _MODULE_FRAMEBUFFER_H

#include "Module.h"

class ComponentMesh;

class ModuleFrameBuffer : public Module
{
public:

	bool CleanUp() override;
	void Start();
	bool RecalcFrameBufferTexture();	
	void Bind() const;
	void UnBind() const;	
	
//members

	unsigned int framebuffer = 0;
	unsigned int texColorBuffer = 0;
	unsigned int depthBuffer = 0;
	unsigned int viewPortWidth = 0;
	unsigned int viewPortHeight = 0;
	unsigned int colorRenderbuffer = 0;
	unsigned int depthRenderbuffer = 0;


};

#endif