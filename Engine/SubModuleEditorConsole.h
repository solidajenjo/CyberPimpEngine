#ifndef _SUB_MODULE_EDITOR_CONSOLE_H
#define _SUB_MODULE_EDITOR_CONSOLE_H

#include "SubModuleEditor.h"

class SubModuleEditorConsole : public SubModuleEditor
{
public:
	SubModuleEditorConsole(char* editorModuleName);
	~SubModuleEditorConsole();

	void Show();

};

#endif