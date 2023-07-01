#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class DeferredHandler
{
private:
	void InitializeGBuffer(ID3D11Device* device, const UINT WIDTH, const UINT HEIGHT, ID3D11Texture2D*& texture, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv);
public:
	DeferredHandler(ID3D11Device* device, const UINT WIDTH, const UINT HEIGHT);
	~DeferredHandler();

	//Textures
	ID3D11Texture2D* positionTexture = nullptr;
	ID3D11Texture2D* normalTexture = nullptr;
	ID3D11Texture2D* ambientTexture = nullptr;
	ID3D11Texture2D* diffuseTexture = nullptr;
	ID3D11Texture2D* specularTexture = nullptr;
	ID3D11Texture2D* shininessTexture = nullptr;

	//RTVs
	ID3D11RenderTargetView* positionRTV = nullptr;
	ID3D11RenderTargetView* normalRTV = nullptr;
	ID3D11RenderTargetView* ambientRTV = nullptr;
	ID3D11RenderTargetView* diffuseRTV = nullptr;
	ID3D11RenderTargetView* specularRTV = nullptr;
	ID3D11RenderTargetView* shininessRTV = nullptr;

	//SRVs
	ID3D11ShaderResourceView* positionSRV = nullptr;
	ID3D11ShaderResourceView* normalSRV = nullptr;
	ID3D11ShaderResourceView* ambientSRV = nullptr;
	ID3D11ShaderResourceView* diffuseSRV = nullptr;
	ID3D11ShaderResourceView* specularSRV = nullptr;
	ID3D11ShaderResourceView* shininessSRV = nullptr;

	//Shaders
	ID3D11VertexShader* deferredVertexShader = nullptr;
	ID3D11PixelShader* deferredPixelShader = nullptr;
	ID3D11ComputeShader* deferredComputeShader = nullptr;
};

