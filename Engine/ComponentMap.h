#ifndef _COMPONENT_MAP_H_
#define _COMPONENT_MAP_H_

#include "Component.h"
#include "MathGeoLib/include/Math/float4.h"
#include <string>
#include <map>

class ComponentMap: public Component
{
public:

	ComponentMap() : Component(ComponentTypes::MAP_COMPONENT) {};

	void EditorDraw() override;
	void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
	bool Release() override;
	ComponentMap* Clone() override;

//members

	unsigned mapId = 0u;
	char mapPath[1024] = "";
	bool useMipMaps = false;

//Resource management
	static ComponentMap* GetMap(const std::string path, bool useMipMaps = false);
	static std::map<std::string, ComponentMap*> mapsLoaded;
};

#endif