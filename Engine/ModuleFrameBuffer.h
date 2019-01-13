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

	unsigned int framebuffer = 0u;
	unsigned int framebufferGame = 0u;
	
	unsigned int texColorBuffer = 0u;
	unsigned int positionBuffer = 0u;
	unsigned int normalsBuffer = 0u;
	unsigned int specularBuffer = 0u;
	unsigned int renderedBuffer = 0u;

	unsigned int depthRenderbuffer = 0u;

	unsigned int viewPortWidth = 0u;
	unsigned int viewPortHeight = 0u;


};

#endif