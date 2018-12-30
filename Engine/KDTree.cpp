#include "KDTree.h"

void KDTNode::SubDivideAABB(unsigned dimension, float median, KDTNode* left, KDTNode* right) const
{
	left->aabb = new AABB();
	right->aabb = new AABB();
	float3 centerLeft = aabb->CenterPoint();
	float3 centerRight = aabb->CenterPoint();
	float3 newSizeLeft = aabb->Size(); 
	float3 newSizeRight = aabb->Size(); 

	switch (dimension)
	{
		case 0:
		{
			float xL = centerLeft.x - newSizeLeft.x * 0.5f;
			newSizeLeft.x = median - xL;
			centerLeft.x = xL + (newSizeLeft.x) * 0.5f;

			float xR = centerRight.x + newSizeRight.x * 0.5f;
			newSizeRight.x = xR - median;
			centerRight.x = xR - (newSizeRight.x) * 0.5f;

			break;
		}
		case 1:
		{
			float yL = centerLeft.y - newSizeLeft.y * 0.5f;
			newSizeLeft.y = median - yL;
			centerLeft.y = yL + (newSizeLeft.y) * 0.5f;

			float yR = centerRight.y + newSizeRight.y * 0.5f;
			newSizeRight.y = yR - median;
			centerRight.y = yR - (newSizeRight.y) * 0.5f;

			break;
		}
		case 2:		
		{
			float zL = centerLeft.z - newSizeLeft.z * 0.5f;
			newSizeLeft.z = median - zL;
			centerLeft.z = zL + (newSizeLeft.z) * 0.5f;

			float zR = centerRight.z + newSizeRight.z * 0.5f;
			newSizeRight.z = zR - median;
			centerRight.z = zR - (newSizeRight.z) * 0.5f;

			break;
		}
	}

	left->aabb->SetFromCenterAndSize(centerLeft, newSizeLeft);
	right->aabb->SetFromCenterAndSize(centerRight, newSizeRight);
}
