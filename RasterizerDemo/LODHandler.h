#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

class LODHandler
{
private:
	bool LoadTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, std::string mapName, bool normal);
	
	void ClearPreviousTexturesAndRaster();
public:
	LODHandler(ID3D11Device* device, std::string mapName);
	~LODHandler();

	ID3D11VertexShader* LODvShader = nullptr;
	ID3D11HullShader* LODhShader = nullptr;
	ID3D11DomainShader* LODdShader = nullptr;
	ID3D11PixelShader* LODpShader = nullptr;
	ID3D11RasterizerState* LODRaster = nullptr;
	
	ID3D11Texture2D* heightTexture = nullptr;
	ID3D11Texture2D* normalTexture = nullptr;
	ID3D11ShaderResourceView* heightSRV = nullptr;
	ID3D11ShaderResourceView* normalSRV = nullptr;
};

