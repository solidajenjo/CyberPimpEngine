#ifndef _KDTREE_H
#define _KDTREE_H

#include "MathGeoLib/include/Geometry/AABB.h"
#include "Application.h"
#include "ModuleScene.h"
#include <vector>
#include "GameObject.h"
#include "debugdraw.h"
#include <queue>
#include <algorithm>

class KDTNode
{
public:

	void SubDivideAABB(unsigned dimension, float median, KDTNode* left, KDTNode* right) const ;

	//members

	KDTNode* upperNode = nullptr;
	unsigned depth = 0u;
	float median = .0f;
	AABB* aabb = nullptr;
	std::vector<GameObject*> bucket;
	
	KDTNode* leftBranch = nullptr;
	KDTNode* rightBranch = nullptr;
	
};

class KDTree
{
public:

	void Calculate();
	void DebugDraw() const;
	template<typename T>
	void GetIntersections(T &intersector, std::vector<GameObject*> &intersections) const;

//members

	KDTNode* treeRoot = nullptr;
	int maxDepth = 6;
};

inline void KDTree::Calculate()
{			
	treeRoot = new KDTNode();
	treeRoot->aabb = new AABB();
	App->scene->GetNonStaticGlobalAABB(treeRoot->aabb, treeRoot->bucket);	
	std::queue<KDTNode*> Q;
	Q.push(treeRoot);
	while (!Q.empty())
	{
		KDTNode* current = Q.front(); Q.pop();
		if (current->bucket.size() > 1u && current->depth <= maxDepth)
		{
			unsigned dimension = current->depth % 3;
			std::sort(current->bucket.begin(), current->bucket.end(), [dimension](const GameObject* go1, const GameObject *go2)
			{
				return go1->transform->getGlobalPosition()[dimension] > go2->transform->getGlobalPosition()[dimension];
				
			});
			if (current->bucket.size() % 2 == 0)
			{
				unsigned middle = current->bucket.size() / 2;
				current->median = (current->bucket[middle - 1]->transform->getGlobalPosition()[dimension] + current->bucket[middle]->transform->getGlobalPosition()[dimension]) / 2.f;
			}
			else
			{
				unsigned middle = current->bucket.size() / 2;
				current->median = current->bucket[middle]->transform->getGlobalPosition()[dimension];
			}
			current->leftBranch = new KDTNode();
			current->leftBranch->depth = current->depth + 1;
			current->rightBranch = new KDTNode();
			current->rightBranch->depth = current->depth + 1;
			current->SubDivideAABB(dimension, current->median, current->leftBranch, current->rightBranch);
			Q.push(current->leftBranch);
			Q.push(current->rightBranch);
			for each(GameObject* go in current->bucket)
			{
				if (current->leftBranch->aabb->ContainsQTree(*go->aaBBGlobal))
				{
					current->leftBranch->bucket.push_back(go);
				}
				if (current->rightBranch->aabb->ContainsQTree(*go->aaBBGlobal))
				{
					current->rightBranch->bucket.push_back(go);
				}
			}
		}
	}

}

inline void KDTree::DebugDraw() const
{
	if (treeRoot == nullptr)
		return;
	std::queue<KDTNode*> Q;
	Q.push(treeRoot);

	while (!Q.empty())
	{
		KDTNode* node = Q.front();
		Q.pop();
		dd::aabb(node->aabb->minPoint, node->aabb->maxPoint, dd::colors::Aquamarine);
		
		if (node->leftBranch != nullptr)
			Q.push(node->leftBranch);
		if (node->rightBranch != nullptr)
			Q.push(node->rightBranch);		
	}
	
}

template<typename T>
inline void KDTree::GetIntersections(T &intersector, std::vector<GameObject*> &intersections) const
{
	std::queue<KDTNode*> Q;
	Q.push(treeRoot);

	while (!Q.empty())
	{
		KDTNode* node = Q.front();
		Q.pop();
		if (node->leftBranch == nullptr && node->aabb->ContainsQTree(intersector)) //check if is not outside
		{
			intersections.insert(intersections.end(), node->bucket.begin(), node->bucket.end());
		}
		if (node->leftBranch != nullptr)
			Q.push(node->leftBranch);
		if (node->rightBranch != nullptr)
			Q.push(node->rightBranch);
	}
}

#endif