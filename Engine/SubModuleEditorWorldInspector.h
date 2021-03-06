#ifndef _SUB_MODULE_EDITOR_WORLD_INSPECTOR_H
#define _SUB_MODULE_EDITOR_WORLD_INSPECTOR_H

#include "SubModuleEditor.h"

class GameObject;

class SubModuleEditorWorldInspector : public SubModuleEditor
{
public:
	SubModuleEditorWorldInspector(const std::string&  editorModuleName) : SubModuleEditor(editorModuleName) {};
	
	void Show();

};

#endif