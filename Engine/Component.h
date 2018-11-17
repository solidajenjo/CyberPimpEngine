#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <string>

class Component
{
public:

	Component(std::string type) : type(type) {}
	virtual ~Component() {};
	virtual void EditorDraw() = 0; //descendants should implement his own inspector handler draw

	//members

	std::string type = "";
};

#endif