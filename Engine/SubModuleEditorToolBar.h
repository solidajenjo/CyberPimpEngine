#ifndef _SUB_MODULE_TOOL_BAR_H
#define _SUB_MODULE_TOOL_BAR_H

#include "SubModuleEditor.h"

class SubModuleEditorToolBar : public SubModuleEditor
{
public:
	SubModuleEditorToolBar(const std::string& editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();

	unsigned toolBarHeight = 60u;
};

#endif