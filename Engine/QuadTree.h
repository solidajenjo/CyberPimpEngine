#ifndef _QUADTREE_H
#define _QUADTREE_H

#include "MathGeoLib/include/Geometry/AABB.h"
#include "Application.h"
#include "ModuleScene.h"
#include <vector>
#include "GameObject.h"
#include "debugdraw.h"
#include <queue>
#include <set>
#include "Brofiler/ProfilerCore/Brofiler.h"

class QTNode
{
public:

	QTNode();
	~QTNode();

	std::vector<QTNode*>& Flush();

	//members
	QTNode* upperNode = nullptr;
	unsigned depth = 0u;
	AABB* aabb = nullptr;
	std::vector<GameObject*> bucket;
	std::vector<QTNode*> lowerNodes;
};

class QuadTree
{
public:

	void Calculate();
	void DebugDraw() const;
	template<typename T>
	void GetIntersections(T &intersector, std::set<GameObject*> &intersections) const; 

//members

	QTNode* treeRoot = nullptr;
	bool showOnEditor = true;
	int maxDepth = 3;
	int bucketSize = 1;
	std::vector<QTNode*> leaves;
};

inline void QuadTree::Calculate()
{			
	RELEASE(treeRoot);
	treeRoot = new QTNode();
	leaves.resize(0);
	App->scene->GetStaticGlobalAABB(treeRoot->aabb, treeRoot->bucket);
	std::queue<QTNode*> Q;
	Q.push(treeRoot);
	while (!Q.empty())
	{
		QTNode* currentNode = Q.front(); Q.pop();
		if (currentNode->bucket.size() > bucketSize && currentNode->depth < maxDepth)
		{
			std::vector<QTNode*> newNodes = currentNode->Flush();
			for each (QTNode* node in newNodes)
			{
				Q.push(node);
			}
		}
		else
			leaves.push_back(currentNode);
	}

}

inline void QuadTree::DebugDraw() const
{
	for (QTNode* node : leaves)
	{
		dd::aabb(node->aabb->minPoint, node->aabb->maxPoint, dd::colors::Aquamarine);
	}
}

template<typename T>
inline void QuadTree::GetIntersections(T &intersector, std::set<GameObject*> &intersections) const
{
	BROFILER_CATEGORY("QuadTree intersections", Profiler::Color::Beige);

	for (QTNode* node : leaves)
	{
		if (node->aabb->ContainsQTree(intersector))
		{
			BROFILER_CATEGORY("QuadTree Set inserts", Profiler::Color::Bisque);
			intersections.insert(node->bucket.begin(), node->bucket.end());
		}
	}

}

#endif