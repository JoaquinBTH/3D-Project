#pragma once
#include <fstream>
#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>
#include <string>
#include <sstream>

using namespace DirectX;

struct Vertex
{
	float pos[3];
	float nrml[3];
	float UV[2];
	int textureUsed;

	Vertex(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT2 UVcoord, int textureIndex)
	{
		pos[0] = position.x;
		pos[1] = position.y;
		pos[2] = position.z;
		nrml[0] = normal.x;
		nrml[1] = normal.y;
		nrml[2] = normal.z;
		UV[0] = UVcoord.x;
		UV[1] = UVcoord.y;
		textureUsed = textureIndex;
	}
};

struct Material
{
	std::string name;
	int index;
	XMFLOAT3 ambientColor;
	XMFLOAT3 diffuseColor;
	XMFLOAT3 specularColor;
	float shininess;
};

struct Submesh
{
	int startIndex;
	int indexCount;

	//Submesh(int startIndex, int indexCount)
	//{
	//	this->startIndex = startIndex;
	//	this->indexCount = indexCount;
	//}
};

struct Object
{
	std::string name;
	Submesh submesh;
};

class ObjectHandler
{
private:
	std::vector<Material> materials;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Object> objects;

	bool LoadMaterial(ID3D11Device* device, const std::string fileName);
	bool LoadTexture(ID3D11Device* device, const std::vector<std::string> maps, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV);

	void CreateBuffers(ID3D11Device* device);

	void ClearPreviousObject();
public:
	ObjectHandler();
	~ObjectHandler();

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Texture2D* mapKdTextureArray = nullptr;
	ID3D11Texture2D* mapKaTextureArray = nullptr;
	ID3D11Texture2D* mapKsTextureArray = nullptr;
	ID3D11ShaderResourceView* mapKdSRV = nullptr;
	ID3D11ShaderResourceView* mapKaSRV = nullptr;
	ID3D11ShaderResourceView* mapKsSRV = nullptr;

	bool LoadObject(ID3D11Device* device, std::string fileName);

	int getIndexCount() const;
	Submesh getSubmesh(int object) const;
};

