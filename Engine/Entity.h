#ifndef _ENTITY_H
#define _ENTITY_H

class Transform;

class Entity
{
	/*
		World instantiable object
	*/
public:
	Entity(const char* name);
	
	~Entity();

	virtual bool Init() {
		return true;
	};

	virtual bool Render() const
	{
		return true;
	};
//members
	const char* entityName;
	Transform* transform;
};

#endif 
