#include "Parser.h"
#include <DirectXMath.h>
#pragma once

using namespace DirectX;

class Octree
{
private:
	std::vector<Octree*> nodeList;
	XMVECTOR positionVec;
	XMFLOAT3 corners[8];
	int depth;
	bool first;
	float size;
	XMFLOAT3 origin;
public:
	Octree(std::vector<Parser*>& objects, XMFLOAT3 position, int depth, float size, bool first);

	std::vector<Parser*> objectsInArea;
	std::vector<int> marked;

	void UpdateItemsAvailable(std::vector<Parser*> objects);

	std::vector<int> getObjectsToRender(const XMFLOAT3& position, const XMVECTOR& viewDir, const XMVECTOR& upDir, const XMVECTOR& rightDir);
};

