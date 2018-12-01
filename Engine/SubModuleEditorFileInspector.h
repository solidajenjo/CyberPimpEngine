#ifndef _SUB_MODULE_FILE_INSPECTOR_H
#define _SUB_MODULE_FILE_INSPECTOR_H

#include "SubModuleEditor.h"
#include <string>
#include <list>

class GameObject;

class SubModuleEditorFileInspector : public SubModuleEditor
{
public:
	SubModuleEditorFileInspector(const std::string& editorModuleName);
	void Show();

};

#endif