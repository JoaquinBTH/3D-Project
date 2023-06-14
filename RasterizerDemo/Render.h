#include "Parser.h"
#include "PipelineHelper.h"
#pragma once
class Render
{
private:

public:
	//Rendering Basics
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Texture2D* dsTexture = nullptr;
	ID3D11DepthStencilView* dsView = nullptr;
	D3D11_VIEWPORT viewport = {};
	ID3D11InputLayout* inputLayout = nullptr;

	Render();
	~Render();

	void StandardRender(ID3D11RenderTargetView* rtv, ID3D11ShaderResourceView* textureSRV, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* vertexBuffer, ID3D11Buffer* matrixConstantBuffer, Parser* baseModel, ID3D11SamplerState* sampler);
	void CubeRender(ID3D11Buffer* vertexBuffer, ID3D11Buffer* currentLightConstBuff[4], ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cubeConstBuffer[6], ID3D11Buffer* cameraPosConstantBuffer, ID3D11Buffer* cubeVertexBuffer);
	void CullingRender(ID3D11Buffer* matrixConstantBuffer, std::vector<int> marked, ID3D11Buffer* frustumVertexBuffer[50], ID3D11ShaderResourceView* frustumSRV[50], std::vector<Parser*>& frustumObjects);
	void LODRender(ID3D11Buffer* vertexBuffer, ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer);
	void ParticleRender(ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer);
};

