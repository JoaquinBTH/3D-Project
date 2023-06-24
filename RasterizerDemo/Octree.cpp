#include "Octree.h"

Octree::Octree(std::vector<ObjectHandler*>& objects, XMFLOAT3 position, int depth, float size, bool first)
{
	this->positionVec = XMVectorSet(position.x, position.y, position.z, 1.0f);
	this->corners[0] = XMFLOAT3(position.x + size, position.y + size, position.z + size);
	this->corners[1] = XMFLOAT3(position.x + size, position.y + size, position.z - size);
	this->corners[2] = XMFLOAT3(position.x + size, position.y - size, position.z + size);
	this->corners[3] = XMFLOAT3(position.x + size, position.y - size, position.z - size);
	this->corners[4] = XMFLOAT3(position.x - size, position.y + size, position.z + size);
	this->corners[5] = XMFLOAT3(position.x - size, position.y + size, position.z - size);
	this->corners[6] = XMFLOAT3(position.x - size, position.y - size, position.z + size);
	this->corners[7] = XMFLOAT3(position.x - size, position.y - size, position.z - size);

	this->depth = depth;
	this->first = first;
	this->size = size;
	this->origin = position;

	if (depth != 0)
	{
		XMFLOAT3 pos;
		for (int i = 0; i < 8; i++)
		{
			switch (i)
			{
			case 0:
				pos = XMFLOAT3(position.x + size / 2, position.y + size / 2, position.z + size / 2);
				break;
			case 1:
				pos = XMFLOAT3(position.x + size / 2, position.y + size / 2, position.z - size / 2);
				break;
			case 2:
				pos = XMFLOAT3(position.x + size / 2, position.y - size / 2, position.z + size / 2);
				break;
			case 3:
				pos = XMFLOAT3(position.x + size / 2, position.y - size / 2, position.z - size / 2);
				break;
			case 4:
				pos = XMFLOAT3(position.x - size / 2, position.y + size / 2, position.z + size / 2);
				break;
			case 5:
				pos = XMFLOAT3(position.x - size / 2, position.y - size / 2, position.z + size / 2);
				break;
			case 6:
				pos = XMFLOAT3(position.x - size / 2, position.y + size / 2, position.z - size / 2);
				break;
			case 7:
				pos = XMFLOAT3(position.x - size / 2, position.y - size / 2, position.z - size / 2);
				break;
			}
			nodeList.push_back(new Octree(objects, pos, depth - 1, size / 2, false));
		}
	}
	else
	{
		UpdateItemsAvailable(objects);
	}
}

void Octree::UpdateItemsAvailable(std::vector<ObjectHandler*> objects)
{
	objectsInArea.clear();

	for (int i = 0; i < objects.size(); i++)
	{
		bool addObject = false;
		/*
		for (int j = 0; j < objects[i]->AmountOfVertices(); j++)
		{
			float position[3];
			objects[i]->giveVertices()[j].GivePosition(position);
			if (this->corners[0].x > position[0] && this->corners[0].y > position[1] && this->corners[0].z > position[2]
				&& this->corners[7].x < position[0] && this->corners[7].y < position[1] && this->corners[7].z < position[2])
			{
				addObject = true;
				break;
			}
		}
		*/

		if (addObject == true)
		{
			this->objectsInArea.push_back(objects[i]);
			this->marked.push_back(i);
		}
	}
}

std::vector<int> Octree::getObjectsToRender(const XMFLOAT3& position, const XMVECTOR& viewDir, const XMVECTOR& upDir, const XMVECTOR& rightDir)
{
	if (this->depth == 0)
	{
		std::vector<int> returnList;

		XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 1.0f);

		XMVECTOR farCenter = pos + viewDir * 1000.0f;
		XMVECTOR farTopLeft = farCenter + (upDir * 500.0f / 2.0f) - (rightDir * 1000.0f / 2.0f);
		XMVECTOR farTopRight = farCenter + (upDir * 500.0f / 2.0f) + (rightDir * 1000.0f / 2.0f);
		XMVECTOR farBottomLeft = farCenter - (upDir * 500.0f / 2.0f) - (rightDir * 1000.0f / 2.0f);
		XMVECTOR farBottomRight = farCenter - (upDir * 500.0f / 2.0f) + (rightDir * 1000.0f / 2.0f);

		XMVECTOR nearCenter = pos + viewDir * 1.0f;
		XMVECTOR nearTopLeft = nearCenter + (upDir * 0.5f / 2.0f) - (rightDir * 1.0f / 2.0f);
		XMVECTOR nearTopRight = nearCenter + (upDir * 0.5f / 2.0f) + (rightDir * 1.0f / 2.0f);
		XMVECTOR nearBottomLeft = nearCenter - (upDir * 0.5f / 2.0f) - (rightDir * 1.0f / 2.0f);
		XMVECTOR nearBottomRight = nearCenter - (upDir * 0.5f / 2.0f) + (rightDir * 1.0f / 2.0f);

		XMVECTOR rightPlanePoint = nearCenter + rightDir * 1.0f / 2.0f;
		XMVECTOR leftPlanePoint = nearCenter - rightDir * 1.0f / 2.0f;
		XMVECTOR topPlanePoint = nearCenter + upDir * 0.5f / 2.0f;
		XMVECTOR bottomPlanePoint = nearCenter - upDir * 0.5f / 2.0f;

		XMVECTOR planePoints[6];
		planePoints[0] = nearCenter;
		planePoints[1] = farCenter;
		planePoints[2] = rightPlanePoint;
		planePoints[3] = leftPlanePoint;
		planePoints[4] = topPlanePoint;
		planePoints[5] = bottomPlanePoint;


		XMVECTOR rightPlaneVec = XMVector3Normalize(rightPlanePoint - pos);
		XMVECTOR leftPlaneVec = XMVector3Normalize(leftPlanePoint - pos);
		XMVECTOR topPlaneVec = XMVector3Normalize(topPlanePoint - pos);
		XMVECTOR bottomPlaneVec = XMVector3Normalize(bottomPlanePoint - pos);

		XMVECTOR planeNormals[6]; //Near, far, right, left, top, bottom

		planeNormals[0] = XMVector3Normalize(-viewDir);
		planeNormals[1] = XMVector3Normalize(viewDir);
		planeNormals[2] = XMVector3Cross(upDir, rightPlaneVec);
		planeNormals[3] = XMVector3Cross(leftPlaneVec, upDir);
		planeNormals[4] = XMVector3Cross(topPlaneVec, rightDir);
		planeNormals[5] = XMVector3Cross(rightDir, bottomPlaneVec);

		//Test check points
		for (int i = ((int)this->objectsInArea.size() - 1); i > -1; i--)
		{
			XMFLOAT3 cornersOfObject[8];
			/*
			cornersOfObject[0] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[0].pos);
			cornersOfObject[1] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[1].pos);
			cornersOfObject[2] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[2].pos);
			cornersOfObject[3] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[3].pos);
			cornersOfObject[4] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[6].pos);
			cornersOfObject[5] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[9].pos);
			cornersOfObject[6] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[18].pos);
			cornersOfObject[7] = XMFLOAT3(this->objectsInArea[i]->giveVertices()[19].pos);
			*/

			bool found = true;

			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					XMVECTOR r = XMVectorSet(-cornersOfObject[i].x, cornersOfObject[i].y, -cornersOfObject[i].z, 1.0f);
					//(v-p)*n > 0.0f or (v-p)*n < 0.0f... v = r, p = planePoint, n = planeNormals
					float vpn = XMVector3Dot((r - planePoints[j]), -planeNormals[j]).m128_f32[0];

					if (vpn < 0.0f)
					{
						found = false;
						break;
					}
				}
				if (found == true)
				{
					break;
				}
			}

			if (found == true)
			{
				returnList.push_back(this->marked[i]);
			}
		}

		return returnList;
	}
	else
	{
		std::vector<int> lists[8];
		for (int i = 0; i < 8; i++)
		{
			lists[i] = nodeList[i]->getObjectsToRender(position, viewDir, upDir, rightDir);
		}

		std::vector<int> returnList;
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < lists[i].size(); j++)
			{
				bool dupe = false;
				for (int h = 0; h < returnList.size(); h++)
				{
					if (returnList[h] == lists[i][j])
					{
						dupe = true;
						break;
					}
				}
				if (dupe == false)
				{
					returnList.push_back(lists[i][j]);
				}
			}
		}
		return returnList;
	}
}
