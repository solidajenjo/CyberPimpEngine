#ifndef _QUADTREE_H
#define _QUADTREE_H

#include "MathGeoLib/include/Geometry/AABB.h"
#include "Application.h"
#include "ModuleScene.h"
#include <vector>
#include "GameObject.h"
#include "debugdraw.h"
#include <queue>

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
	void GetIntersections(T &intersector, std::vector<GameObject*> &intersections) const;

//members

	QTNode* treeRoot = nullptr;
	int maxDepth = 6;
};

inline void QuadTree::Calculate()
{			
	treeRoot = new QTNode();
	App->scene->GetStaticGlobalAABB(treeRoot->aabb, treeRoot->bucket);
	std::queue<QTNode*> Q;
	Q.push(treeRoot);
	while (!Q.empty())
	{
		QTNode* currentNode = Q.front(); Q.pop();
		if (currentNode->bucket.size() > 1 && currentNode->depth < maxDepth)
		{
			std::vector<QTNode*> newNodes = currentNode->Flush();
			for each (QTNode* node in newNodes)
			{
				Q.push(node);
			}
		}
	}

}

inline void QuadTree::DebugDraw() const
{
	if (treeRoot == nullptr)
		return;
	std::queue<QTNode*> Q;
	Q.push(treeRoot);

	while (!Q.empty())
	{
		QTNode* node = Q.front(); 
		Q.pop();
		if (node->lowerNodes.size() == 0u)
			dd::aabb(node->aabb->minPoint, node->aabb->maxPoint, dd::colors::Aquamarine);
		for each (QTNode* n in node->lowerNodes)
		{
			Q.push(n);
		}
	}

}

template<typename T>
inline void QuadTree::GetIntersections(T &intersector, std::vector<GameObject*> &intersections) const
{
	std::queue<QTNode*> Q;
	Q.push(treeRoot);

	while (!Q.empty())
	{
		QTNode* node = Q.front();
		Q.pop();
		if (node->lowerNodes.size() == 0u && node->aabb->ContainsQTree(intersector))
		{
			intersections.insert(intersections.end(), node->bucket.begin(), node->bucket.end());
		}
		else
		{
			for each (QTNode* n in node->lowerNodes)
			{
				Q.push(n);
			}
		}
	}
}

#endif