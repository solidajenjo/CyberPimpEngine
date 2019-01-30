#include "ModuleGamePlaySystem.h"
#include <Windows.h>
#include "../DLLGamePlaySystem/DLLGamePlaySystem.h"

bool ModuleGamePlaySystem::Init()
{
	return true;
}

update_status ModuleGamePlaySystem::Update()
{	
/*	HINSTANCE dll;
	SET_CALLBACK SetCallBack;
	HELLO_WORLD HelloWorld;

	// Get a handle to the DLL module.
	
	dll = LoadLibrary("C:\\Users\\derek\\Desktop\\DraconisEngine\\Debug\\DLLGamePlaySystem.dll");

	// If the handle is valid, try to get the function address.

	if (dll != nullptr)
	{
		SetCallBack = (SET_CALLBACK)GetProcAddress(dll, TEXT("SetLogCallBack"));
		if (SetCallBack != nullptr)
		{
			SetCallBack(&ModuleGamePlaySystem::LogDll);
		}	

		HelloWorld = (HELLO_WORLD)GetProcAddress(dll, TEXT("HelloWorld"));
		if (HelloWorld != nullptr)
		{
			HelloWorld();
		}
		// Free the DLL module.

		FreeLibrary(dll);
	}
	*/
	return UPDATE_CONTINUE;
}

void ModuleGamePlaySystem::LogDll(const char * msg)
{
	LOG("%s", msg);
}
