#include "ObjectHandler.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Camera.h"
#pragma once

using namespace DirectX;

struct LeafNode
{
	BoundingBox octant;
	std::vector<int> leafObjectIndices;
};

class OctreeHandler
{
private:
	std::vector<LeafNode> leafNodes;

	void RecursiveFill(int depth, XMFLOAT3 origin, float size);
public:
	OctreeHandler(ID3D11Device* device, std::string objName, XMFLOAT3 origin, int depth, float size);
	~OctreeHandler();

	ObjectHandler* scene = nullptr;
	std::vector<int> objectIndices;

	void FrustumCulling(const Camera* camera);
};

