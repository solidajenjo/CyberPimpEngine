#ifndef _MODULE_SPACE_PARTITIONING_H
#define _MODULE_SPACE_PARTITIONING_H

#include "Module.h"
#include "QuadTree.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"

class ModuleSpacePartitioning :
	public Module
{
public:

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	//members

	QuadTree quadTree;
	

};

#endif
