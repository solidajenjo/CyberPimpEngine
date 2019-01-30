#ifndef __DLL_GAMEPLAYSYSTEM_H_
#define __DLL_GAMEPLAYSYSTEM_H_

#define DECLDIR __declspec(dllexport)

#include "stdafx.h"
#include <stdarg.h> 
#include "DLLGamePlaySystemDeclarations.h"

LOG_CB LogCallback = nullptr;

extern "C"
{
	DECLDIR bool SetLogCallBack(LOG_CB callback)
	{
		LogCallback = callback;
		return LogCallback != nullptr;
	}

	DECLDIR void HelloWorld()
	{
		LogCallback("HelloWorld Oh my God");
	}
}

#endif