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

template<typename T>
class QuadTree
{
public:

	void Calculate();
	void DebugDraw();

//members

	QTNode* treeRoot = nullptr;
	int maxDepth = 1;
};

template<typename T>
inline void QuadTree<T>::Calculate()
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

template<typename T>
inline void QuadTree<T>::DebugDraw()
{
	if (treeRoot == nullptr)
		return;
	std::queue<QTNode*> Q;
	Q.push(treeRoot);

	while (!Q.empty())
	{
		QTNode* node = Q.front(); 
		Q.pop();
		//if (node->lowerNodes.size() == 0)
			dd::aabb(node->aabb->minPoint, node->aabb->maxPoint, dd::colors::Aquamarine);
		for each (QTNode* n in node->lowerNodes)
		{
			Q.push(n);
		}
	}

}
#endif