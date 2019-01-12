#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <list>
#include <string>
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"
#include "rapidjson-1.1.0/include/rapidjson/prettywriter.h"
#include "rapidjson-1.1.0/include/rapidjson/document.h"

class ComponentMesh;
class Transform;
class Component;
class AABBTreeNode;

class GameObject
{
public:

	enum class GameObjectLayers
	{
		DEFAULT,			// General layer
		WORLD_VOLUME,		// QuadTree / KDTree / AABBTree detectable - Used to discard lights, cameras & other non volumetric instantiated gameobjects
		LIGHTING			// AABBTree lights layer to lighting frustum culling
	};

	GameObject(const char name[1024], bool isContainer = false);

	GameObject(char UUID[40], Transform* transform);

	~GameObject();
	
	void InsertComponent(Component* newComponent);
	void InsertChild(GameObject* child);
	void SetInstanceOf(char instanceOrigin[40]);
	bool RayAgainstMeshNearestHitPoint(const LineSegment &lSeg, float3 &hitPoint) const;

	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
	bool UnSerialize(rapidjson::Value &value);	

	void PropagateStaticCheck();
	
	GameObject* MakeInstanceOf() const;
	GameObject* Clone(bool breakInstance = false) const;

	//members
	
	Transform* transform = nullptr;

	GameObject* parent = nullptr; //Released by scene module

	std::list<GameObject*> children; //Released by scene module (each one)
	std::list<Component*> components;
	
	GameObjectLayers layer = GameObjectLayers::DEFAULT;

	AABB* aaBB = nullptr;
	AABB* aaBBGlobal = nullptr;
	bool enabled = true, selected = false, isRenderizable = false;
	
	char name[1024] = "";
	char gameObjectUUID[40] = ""; //unique game object id
	char parentUUID[40] = ""; //unique parent's game object id
	char instanceOf[40] = ""; //identifier to search on assets hierarchy & clone
	char path[1024] = ""; //some type of gameobjects store a path to be loaded from disk
	bool isInstantiated = false;
	bool isStatic = false;
	bool isContainer = false;
	bool active = true;
	bool isFake = false;
	bool hasLights = false;
	GameObject* fakeGameObjectReference = nullptr;

	AABBTreeNode* treeNode = nullptr; //Pointer to the AABBTree node who holds the gameobject
};


#endif