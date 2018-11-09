#ifndef _SUB_MODULE_EDITOR_MENU_H
#define _SUB_MODULE_EDITOR_MENU_H

#include "SubModuleEditor.h"

class SubModuleEditorMenu : public SubModuleEditor
{
public:
	SubModuleEditorMenu(std::string editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();
};

#endif