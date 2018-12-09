#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <string>
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"

class GameObject;

class Component
{
public:

	enum class ComponentTypes
	{
		VOID_COMPONENT = -1,		
		CAMERA_COMPONENT = 0,
		MESH_COMPONENT,
		MATERIAL_COMPONENT,
		TRANSFORM_COMPONENT
	};


	Component(ComponentTypes type) : type(type) {}
		
	virtual ~Component() {};
	virtual void EditorDraw() = 0; //descendants should implement his own inspector handler draw
	virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) { }
	virtual void UnSerialize(rapidjson::Value &value) {}
	virtual bool Release() { return true; } // tells if it has to be destroyed if it has no active clients on release
	//members

	ComponentTypes type = ComponentTypes::VOID_COMPONENT;
	GameObject* owner = nullptr;

	unsigned clients = 0u; //controls shared component clean
};

#endif