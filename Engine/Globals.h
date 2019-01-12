#ifndef _GLOBALS_H
#define _GLOBALS_H


#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__)
#define RELEASE(p) { \
	if (p != nullptr) \
	{ \
		delete p; \
		p = nullptr; \
	} \
}

#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }


#define PROCESS_PRIMITIVE(){\
								newMesh->material->owner = newGO;\
								App->renderer->insertRenderizable(newGO);\
								newMesh->SendToGPU();\
								newGO->InsertComponent(newMesh);\
								newGO->isInstantiated = true; \
								App->scene->AttachToRoot(newGO);\
								newGO->transform->PropagateTransform();\
								App->scene->InsertGameObject(newGO);\
								ImGui::CloseCurrentPopup(); }

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define FULLSCREEN false
#define RESIZABLE_WINDOW true
#define TITLE "Draconis Engine"
#define OPENGL_VERSION "#version 330"
#define MAX_NON_STATIC_GAMEOBJECTS 4096
#endif