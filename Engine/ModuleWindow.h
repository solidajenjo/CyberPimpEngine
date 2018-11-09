#ifndef _MODULE_WINDOW_H
#define _MODULE_WINDOW_H

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;
class Application;

class ModuleWindow : public Module
{
public:


	// Called before quitting
	bool Init() override;

	// Called before quitting
	bool CleanUp() override;

//members

	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	int screenWidth = 0, screenHeight = 0;
};

#endif 