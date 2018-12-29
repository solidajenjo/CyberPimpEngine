#include "KDTree.h"

void KDTNode::SubDivideAABB(unsigned dimension, KDTNode* left, KDTNode* right) const
{
	left->aabb = new AABB();
	right->aabb = new AABB();
	float3 centerLeft = aabb->CenterPoint();
	float3 centerRight = aabb->CenterPoint();
	float3 newSize = aabb->Size(); 

	switch (dimension)
	{
	case 0:
		newSize.x *= .5f;
		centerLeft.x -= newSize.x * .5f;
		centerRight.x += newSize.x * .5f;
		break;
	case 1:
		newSize.y *= .5f;
		centerLeft.y -= newSize.y * .5f;
		centerRight.y += newSize.y * .5f;
		break;
	case 2:
		newSize.z *= .5f;
		centerLeft.z -= newSize.z * .5f;
		centerRight.z += newSize.z * .5f;
		break;
	}

	left->aabb->SetFromCenterAndSize(centerLeft, newSize);
	right->aabb->SetFromCenterAndSize(centerRight, newSize);
}
