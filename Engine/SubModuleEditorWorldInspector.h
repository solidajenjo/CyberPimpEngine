#ifndef _SUB_MODULE_EDITOR_WORLD_INSPECTOR_H
#define _SUB_MODULE_EDITOR_WORLD_INSPECTOR_H

#include "SubModuleEditor.h"

class SubModuleEditorWorldInspector : public SubModuleEditor
{
public:
	SubModuleEditorWorldInspector(char* editorModuleName);
	~SubModuleEditorWorldInspector();

	void Show();

};

#endif