#pragma once

#include "MtlParser.h"

struct Vertex
{
	float pos[3];
	float nrml[3];
	float UV[2];
	int textureUsed;

	//float Ns = 0.0f;
	//float Ka[3] = {0.0f, 0.0f, 0.0f};
	//float Kd[3] = {0.0f, 0.0f, 0.0f};
	//float Ks[3] = {0.0f, 0.0f, 0.0f};
	//float Ke[3] = {0.0f, 0.0f, 0.0f};
	//float Ni = 0.0f;
	//float d = 0.0f;

	Vertex(Vector3 poscoord, Vector3 nrmlcoord, Vector2 UVcoord, int texture)
	{
		pos[0] = poscoord.GetX();
		pos[1] = poscoord.GetY();
		pos[2] = poscoord.GetZ();
		nrml[0] = nrmlcoord.GetX();
		nrml[1] = nrmlcoord.GetY();
		nrml[2] = nrmlcoord.GetZ();
		UV[0] = UVcoord.GetX();
		UV[1] = UVcoord.GetY();
		textureUsed = texture;
	}

	/*
	void AddMtlValues(float NsVal, float KaVal[3], float KdVal[3], float KsVal[3], float KeVal[3], float NiVal, float dVal)
	{
		Ns = NsVal;
		Ka[0] = KaVal[0];
		Ka[1] = KaVal[1];
		Ka[2] = KaVal[2];
		Kd[0] = KdVal[0];
		Kd[1] = KdVal[1];
		Kd[2] = KdVal[2];
		Ks[0] = KsVal[0];
		Ks[1] = KsVal[1];
		Ks[2] = KsVal[2];
		Ke[0] = KeVal[0];
		Ke[1] = KeVal[1];
		Ke[2] = KeVal[2];
		Ni = NiVal;
		d = dVal;
	}
	*/

	void GivePosition(float fill[3])
	{
		for (int i = 0; i < 3; i++)
		{
			fill[i] = pos[i];
		}
	}

	void OffSetPosition(float offSet)
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] += offSet;
		}
	}
};

struct Index
{
	int startIndex = 0;
	int endIndex = 0;

	Index(int start, int end)
	{
		startIndex = start;
		endIndex = end;
	}

	int start()
	{
		return startIndex;
	}

	int end()
	{
		return endIndex;
	}

	int numberOfIndices()
	{
		return (endIndex - startIndex) + 1;
	}
};

class Parser
{
private:

	int amountOfObjects = 0;
	std::string mtllib = "";
	std::vector<std::string> materialName;

	//OBJ files have "f" which means faces. So f 1/1/1 5/2/1 7/3/1 3/4/1 means that the first face has vertices with the combination v/vt/vn corresponding to the numbers.

public:

	Parser(std::string filePath);
	
	std::vector<Vertex> vertices;
	std::vector<Vertex> testVertices;
	std::vector<Index> indices;
	std::vector<int> indexOrder;

	MtlParser* mtlParser;

	int sizeOfVertexBuffer() const;

	int AmountOfVertices() const;

	std::vector<Vertex> giveVertices() const;

	std::vector<Materialinfo> GiveMaterialInfo() const;

	std::vector<D3D11_SUBRESOURCE_DATA> GiveData() const;

	void updateVerticesPos(float offSet);
};

