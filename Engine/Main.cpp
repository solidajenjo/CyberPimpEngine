#include <stdlib.h>
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "SubModuleEditorViewPort.h"
#include "Globals.h"
#include "Time.h"
#include "SDL.h"

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	Time time;
	while (state != MAIN_EXIT)
	{	
		switch (state)
		{
		case MAIN_CREATION:

			time.StartMS();
			LOG("Application Creation --------------");
			App = new Application();
			state = MAIN_START;
			time.StopMS();
			LOG("Application creation took %d ms\n", time.ReadMS());
			break;

		case MAIN_START:

			LOG("Application Init --------------");
			time.StartMS();
			time.StartUS();
			if (App->Init() == false)
			{
				LOG("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("Application Update --------------");
				
			}
			time.StopMS();
			time.StopUS();
			LOG("Application Init took %d ms <-> %d microSeconds \n", time.ReadMS(), time.ReadUS());
			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG("Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with error -----");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
		if (App->exit && state != MAIN_EXIT) state = MAIN_FINISH;
	}

	delete App;
	LOG("Bye :)\n");
	return main_return;
}
