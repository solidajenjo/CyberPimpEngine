#ifndef _SUB_MODULE_EDITOR_CONSOLE_H
#define _SUB_MODULE_EDITOR_CONSOLE_H

#include "SubModuleEditor.h"

class SubModuleEditorConsole : public SubModuleEditor
{
public:
	SubModuleEditorConsole(std::string editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();

};

#endif