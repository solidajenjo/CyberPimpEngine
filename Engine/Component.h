#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <string>
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"

class GameObject;

class Component
{
public:

	enum class ComponentTypes
	{
		CAMERA_COMPONENT = 0,
		MESH_COMPONENT,
		MATERIAL_COMPONENT
	};


	Component(std::string type_str) {
		sprintf_s(type, type_str.c_str());
	}
	virtual ~Component() {};
	virtual void EditorDraw() = 0; //descendants should implement his own inspector handler draw
	virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) { }

	//members

	char type[25] = "" ;
	GameObject* owner = nullptr;
};

#endif