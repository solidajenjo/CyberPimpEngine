#ifndef _SUB_MODULE_GAME_VIEWPORT_H
#define _SUB_MODULE_GAME_VIEWPORT_H

#include "SubModuleEditor.h"
#include "ModuleTextures.h"

class ModuleFrameBuffer;

class SubModuleEditorGameViewPort : public SubModuleEditor
{
public:

	enum class AntiaAliasing
	{
		NONE = 0,
		SSAA2,
		MSAA4,
	};

	SubModuleEditorGameViewPort(const std::string& editorModuleName) : SubModuleEditor(editorModuleName) {};

	void Show();

//members

	float height = 0, width = 0;
	AntiaAliasing antialiasing = AntiaAliasing::NONE;
	bool framebufferDirty = false;
};

#endif