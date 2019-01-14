#include "Globals.h"
#include "Application.h"
#include "ModuleFrameBuffer.h"
#include "ComponentMesh.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Util/par_shapes.h"
#include "glew-2.1.0/include/GL/glew.h"


void ModuleFrameBuffer::Start()
{
	if (framebuffer == 0)
	{
		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &depthRenderbuffer);
		// generate textures
		glGenTextures(1, &renderedBuffer);
		glGenTextures(1, &positionBuffer);
		glGenTextures(1, &normalsBuffer);
		glGenTextures(1, &specularBuffer);
		glGenTextures(1, &texColorBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}



// Called before quitting
bool ModuleFrameBuffer::CleanUp()
{
	LOG("Cleaning Module Framebuffer");
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &texColorBuffer); //TODO:delete textures
	LOG("Cleaning Module Framebuffer. Done"); //TODO:create framebuffer object
	return true;
}

bool ModuleFrameBuffer::RecalcFrameBufferTexture()
{
	bool ret = true;

	if (App->renderer->renderMode == ModuleRender::RenderMode::DEFERRED)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, viewPortWidth, viewPortHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderedBuffer);

		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewPortWidth, viewPortHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		glBindTexture(GL_TEXTURE_2D, renderedBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewPortWidth, viewPortHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedBuffer, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, positionBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewPortWidth, viewPortHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positionBuffer, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, normalsBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewPortWidth, viewPortHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalsBuffer, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, specularBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewPortWidth, viewPortHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, specularBuffer, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewPortWidth, viewPortHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, texColorBuffer, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, attachments);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, viewPortWidth, viewPortHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderedBuffer);

		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewPortWidth, viewPortHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		glBindTexture(GL_TEXTURE_2D, renderedBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewPortWidth, viewPortHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedBuffer, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);
	}
	

	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG("Failed to make complete framebuffer object %x", status);
		ret = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return ret;
}

void ModuleFrameBuffer::Bind() const
{
	if (framebuffer != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, viewPortWidth, viewPortHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}
}

void ModuleFrameBuffer::UnBind() const
{		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

