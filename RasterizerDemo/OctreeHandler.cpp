#include "OctreeHandler.h"

void OctreeHandler::RecursiveFill(int depth, XMFLOAT3 origin, float size)
{
	if (depth > 0)
	{
		//Create a smaller octant for each direction until depth is 0
		float halfSize = size * 0.5f;
		float quarterSize = size * 0.25f;

		for (int i = 0; i < 8; i++)
		{
			XMFLOAT3 nodeOrigin = origin;

			//Create a new origin for each new node
			if (i < 4) nodeOrigin.x += quarterSize; //First 4 nodes are positive on x
			else nodeOrigin.x -= quarterSize; //Last 4 nodes are negative on x

			if (i % 4 < 2) nodeOrigin.y += quarterSize; //Indices 0,1 and 4,5 are positive on y
			else nodeOrigin.y -= quarterSize; //Indicies 2,3 and 6,7 are negative on y

			if (i % 2 == 0) nodeOrigin.z += quarterSize; //indices 0,2,4,6 are positive on z
			else nodeOrigin.z -= quarterSize; //Indicies 1,3,5,7 are negative on z

			RecursiveFill(depth - 1, nodeOrigin, halfSize);
		}
	}
	else
	{
		//Create LeafNode
		LeafNode leafNode;

		float halfSize = size * 0.5f;
		
		//Create minPoint and maxPoint of a BoundingBox
		XMFLOAT3 minPoint, maxPoint;

		minPoint.x = origin.x - halfSize;
		minPoint.y = origin.y - halfSize;
		minPoint.z = origin.z - halfSize;

		maxPoint.x = origin.x + halfSize;
		maxPoint.y = origin.y + halfSize;
		maxPoint.z = origin.z + halfSize;

		BoundingBox::CreateFromPoints(leafNode.octant, XMVectorSet(minPoint.x, minPoint.y, minPoint.z, 0.0f), XMVectorSet(maxPoint.x, maxPoint.y, maxPoint.z, 0.0f));

		//Check which objects are within the boundingBox of the LeafNode
		for (int i = 0; i < this->scene->getObjects().size(); i++)
		{
			for (int j = 0; j < this->scene->getObjects()[i].submesh.indexCount; j++)
			{
				//Check for each vertex of the object
				unsigned int vertexIndex = this->scene->getIndices()[static_cast<size_t>(this->scene->getObjects()[i].submesh.startIndex) + static_cast<size_t>(j)];
				XMFLOAT3 point = XMFLOAT3(this->scene->getVertices()[vertexIndex].pos[0],
										this->scene->getVertices()[vertexIndex].pos[1],
										this->scene->getVertices()[vertexIndex].pos[2]);

				//If vertex is within the bounding box, add it to the list and break
				if (leafNode.octant.Contains(XMLoadFloat3(&point)))
				{
					leafNode.leafObjectIndices.push_back(i);
					break;
				}
			}
		}

		this->leafNodes.push_back(leafNode);
	}
}

OctreeHandler::OctreeHandler(ID3D11Device* device, std::string objName, XMFLOAT3 origin, int depth, float size)
{
	//Load the scene
	this->scene = new ObjectHandler();
	this->scene->LoadObject(device, objName);

	//Fill the leaf nodes with the objects they will contain
	RecursiveFill(depth, origin, size);
}

OctreeHandler::~OctreeHandler()
{
	if (scene) delete scene;
}

void OctreeHandler::FrustumCulling(const Camera* camera)
{
	this->objectIndices.clear();

	//Create frustum
	BoundingFrustum frustum;
	frustum.CreateFromMatrix(frustum, camera->GetProjectionMatrix());

	//Correct the origin and the orientation of the frustum
	XMFLOAT3 cameraPosition = camera->GetPositionFloat3();
	frustum.Origin = cameraPosition;
	XMFLOAT3 cameraRotation = camera->GetRotationFloat3();
	XMVECTOR orientationQuaternion = XMQuaternionRotationRollPitchYaw(cameraRotation.x, cameraRotation.y, cameraRotation.z);
	XMStoreFloat4(&frustum.Orientation, orientationQuaternion);

	//For every leaf node
	for (int i = 0; i < leafNodes.size(); i++)
	{
		//Check if the leaf node intersects the view-frustum
		if (frustum.Intersects(leafNodes[i].octant))
		{
			//For every leafObjectIndex
			for (int j = 0; j < leafNodes[i].leafObjectIndices.size(); j++)
			{
				int index = leafNodes[i].leafObjectIndices[j];
				//Check if index has already been added to objectIndices
				bool duplicate = false;
				for (int k = 0; k < this->objectIndices.size(); k++)
				{
					if (index == this->objectIndices[k])
					{
						duplicate = true;
						break;
					}
				}

				//If it's not a duplicate, add it to the list
				if (!duplicate)
				{
					this->objectIndices.push_back(index);
				}
			}
		}
	}
}