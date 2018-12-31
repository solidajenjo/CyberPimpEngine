#ifndef _SUB_MODULE_EDITOR_MENU_H
#define _SUB_MODULE_EDITOR_MENU_H

#include "SubModuleEditor.h"

class FileExplorer;

class SubModuleEditorMenu : public SubModuleEditor
{
public:

	enum class MenuOperations
	{
		NONE,
		LOAD,
		SAVE
	};

	SubModuleEditorMenu(const std::string& editorModuleName);
	~SubModuleEditorMenu();

	void Show();

	//members

	FileExplorer* fileExplorer = nullptr;
	MenuOperations currentOperation = MenuOperations::NONE;
};

#endif