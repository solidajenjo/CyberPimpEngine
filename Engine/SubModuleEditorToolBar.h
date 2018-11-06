#ifndef _SUB_MODULE_TOOL_BAR_H
#define _SUB_MODULE_TOOL_BAR_H

#include "SubModuleEditor.h"

class SubModuleEditorToolBar : public SubModuleEditor
{
public:
	SubModuleEditorToolBar(char* editorModuleName);
	~SubModuleEditorToolBar();

	void Show() ;

	unsigned toolBarHeight;
};

#endif