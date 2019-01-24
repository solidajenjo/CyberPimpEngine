#pragma once

typedef bool(__cdecl *SET_CALLBACK)(...);

typedef void(WINAPI *LOG_CB)(const char *msg);
typedef void(WINAPI *HELLO_WORLD)(void);

