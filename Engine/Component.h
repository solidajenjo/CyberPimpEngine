#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class Component
{
public:

	virtual ~Component() {};
	virtual void EditorDraw() = 0; //descendants should implement his own inspector handler draw
};

#endif