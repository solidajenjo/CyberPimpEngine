#ifndef _SUB_MODULE_GAME_VIEWPORT_H
#define _SUB_MODULE_GAME_VIEWPORT_H

#include "SubModuleEditor.h"
#include "ModuleTextures.h"
class SubModuleEditorGameViewPort : public SubModuleEditor
{
public:
	SubModuleEditorGameViewPort(const std::string& editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();

//members

	float height = 0, width = 0;
	bool cursorIn = false;
};

#endif