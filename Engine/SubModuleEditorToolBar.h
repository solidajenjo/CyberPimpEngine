#ifndef _SUB_MODULE_TOOL_BAR_H
#define _SUB_MODULE_TOOL_BAR_H

#include "SubModuleEditor.h"

class SubModuleEditorToolBar : public SubModuleEditor
{
public:
	SubModuleEditorToolBar(const std::string& editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();

	//members

	unsigned toolBarHeight = 60u;

	const char* aa_items[2] = { "None", "SSAAx2" };
	const char* scale_items[4] = { "1", "10", "100", "1000" };
	const char* scale_item_current = scale_items[0];
	const char* aa_item_current = aa_items[0];
};

#endif