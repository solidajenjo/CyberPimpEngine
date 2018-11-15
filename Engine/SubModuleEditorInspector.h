#ifndef _SUB_MODULE_EDITOR_INSPECTOR_H
#define _SUB_MODULE_EDITOR_INSPECTOR_H

#include "SubModuleEditor.h"

class SubModuleEditorInspector : public SubModuleEditor
{
public:
	SubModuleEditorInspector(const std::string&  editorModuleName) : SubModuleEditor(editorModuleName) {};
	
	void Show();

};

#endif