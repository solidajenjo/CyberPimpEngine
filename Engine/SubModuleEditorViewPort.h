#ifndef _SUB_MODULE_EDITOR_VIEWPORT_H
#define _SUB_MODULE_EDITOR_VIEWPORT_H

#include "SubModuleEditor.h"
#include "ModuleTextures.h"
class SubModuleEditorViewPort : public SubModuleEditor
{
public:
	SubModuleEditorViewPort(char* editorModuleName);
	~SubModuleEditorViewPort();

	void Show();

	void Init();

	unsigned int framebuffer;
	unsigned int texColorBuffer;

};

#endif