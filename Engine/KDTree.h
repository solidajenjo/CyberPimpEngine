#ifndef _KDTREE_H
#define _KDTREE_H

#include "MathGeoLib/include/Geometry/AABB.h"
#include "Application.h"
#include "ModuleScene.h"
#include <vector>
#include <set>
#include "GameObject.h"
#include "debugdraw.h"
#include <queue>
#include <algorithm>
#include "Brofiler/ProfilerCore/Brofiler.h"

#define BUCKET_MAX 1024
#define MAX_LEAVES 1024

class KDTNode
{
public:

	~KDTNode();
	void SubDivideAABB(unsigned dimension, float median, KDTNode* left, KDTNode* right) const ;

	//members

	KDTNode* upperNode = nullptr;
	unsigned depth = 0u;
	float median = .0f;
	AABB* aabb = nullptr;
	std::vector<GameObject*> bucket;
	unsigned bucketOccupation = 0u;
	
	KDTNode* leftBranch = nullptr;
	KDTNode* rightBranch = nullptr;
	
};

class KDTree
{
public:

	~KDTree();

	void Init();
	void Calculate();
	void DebugDraw() const;
	template<typename T>
	void GetIntersections(T &intersector, std::set<GameObject*> &intersections) const;

//members

	std::vector<KDTNode*> leaves;
	unsigned leavesAmount = 0u;
	KDTNode* treeRoot = nullptr;
	bool showOnEditor = true;
	int maxDepth = 6;
	int bucketSize = 5;
};

inline KDTree::~KDTree()
{
	RELEASE(treeRoot);
}

inline void KDTree::Init()
{
	LOG("Initializating KDTree with depth %d", maxDepth);
	RELEASE(treeRoot);	
	std::queue<KDTNode*> Q;
	treeRoot = new KDTNode();
	treeRoot->aabb = new AABB();
	Q.push(treeRoot);
	while (!Q.empty())
	{
		KDTNode* current = Q.front(); Q.pop();
		current->bucket.resize(BUCKET_MAX);
		if (current->depth <= maxDepth)
		{
			current->leftBranch = new KDTNode();
			current->rightBranch = new KDTNode();
			current->leftBranch->aabb = new AABB();
			current->rightBranch->aabb = new AABB();
			current->leftBranch->depth = current->depth + 1;
			current->rightBranch->depth = current->depth + 1;
			Q.push(current->leftBranch);
			Q.push(current->rightBranch);
		}
	}
	leaves.resize(MAX_LEAVES);
}

inline void KDTree::Calculate()
{			
	BROFILER_CATEGORY("Calculate KD-Tree", Profiler::Color::AliceBlue);	
	treeRoot->bucketOccupation = 0u;
	App->scene->GetNonStaticGlobalAABB(treeRoot->aabb, treeRoot->bucket, treeRoot->bucketOccupation);	
	std::queue<KDTNode*> Q;
	leavesAmount = 0u;
	Q.push(treeRoot);
	while (!Q.empty())
	{
		KDTNode* current = Q.front(); Q.pop();		
		if (current->bucketOccupation > bucketSize && current->depth <= maxDepth)
		{
			unsigned dimension = current->depth % 3;
			std::sort(current->bucket.begin(), current->bucket.begin() + current->bucketOccupation, [dimension](const GameObject* go1, const GameObject *go2)
			{
				if (go2 == nullptr) // odd bucket occupation
					return false;
				return go1->transform->getGlobalPosition()[dimension] > go2->transform->getGlobalPosition()[dimension];				
			});
			if (current->bucketOccupation % 2 == 0)
			{
				unsigned middle = (current->bucketOccupation * 0.5f) + 1u; //the first is always null due the preincrement on filling the bucket
				current->median = (current->bucket[middle - 1]->transform->getGlobalPosition()[dimension] + current->bucket[middle]->transform->getGlobalPosition()[dimension]) / 2.f;
			}
			else
			{
				unsigned middle = (current->bucketOccupation * 0.5f) + 1u; //the first is always null due the preincrement on filling the bucket
				current->median = current->bucket[middle]->transform->getGlobalPosition()[dimension];
			}
			current->leftBranch->bucketOccupation = 0u;
			current->rightBranch->bucketOccupation = 0u;
			
			current->SubDivideAABB(dimension, current->median, current->leftBranch, current->rightBranch);			
			Q.push(current->leftBranch);
			Q.push(current->rightBranch);
			
			for (unsigned i = 1u; i <= current->bucketOccupation; ++i) //the first is always null due the preincrement on filling the bucket
			{
				if (current->leftBranch->aabb->ContainsQTree(*current->bucket[i]->aaBBGlobal))
				{
					current->leftBranch->bucket[++current->leftBranch->bucketOccupation] = current->bucket[i];
				}
				if (current->rightBranch->aabb->ContainsQTree(*current->bucket[i]->aaBBGlobal))
				{
					current->rightBranch->bucket[++current->rightBranch->bucketOccupation] = current->bucket[i];
				}
			}
		}
		else
		{
			leaves[++leavesAmount] = current;
		}
	}
}

inline void KDTree::DebugDraw() const
{
	for (unsigned i = 1u; i <= leavesAmount; ++i)
	{
		
		switch (leaves[i]->depth % 3)
		{
		case 0:
			dd::aabb(leaves[i]->aabb->minPoint * .99f, leaves[i]->aabb->maxPoint * .99f, dd::colors::Red);
			break;
		case 1:
			dd::aabb(leaves[i]->aabb->minPoint * .99f, leaves[i]->aabb->maxPoint * .99f, dd::colors::Green);
			break;
		case 2:
			dd::aabb(leaves[i]->aabb->minPoint * .99f, leaves[i]->aabb->maxPoint * .99f, dd::colors::Blue);
			break;
		}
	}	
}

template<typename T>
inline void KDTree::GetIntersections(T &intersector, std::set<GameObject*> &intersections) const
{
	BROFILER_CATEGORY("KDTree intersections", Profiler::Color::Azure);
	assert(leavesAmount < MAX_LEAVES);
	for (unsigned i = 1u; i <= leavesAmount; ++i)
	{		
		if (leaves[i]->aabb->ContainsQTree(intersector)) //check if is not outside
		{
			intersections.insert(leaves[i]->bucket.begin() + 1, leaves[i]->bucket.begin() + leaves[i]->bucketOccupation + 1); //the first is always null due the preincrement on filling the bucket
		}
	}
}

#endif