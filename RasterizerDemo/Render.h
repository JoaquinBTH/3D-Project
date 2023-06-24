//#include "PipelineHelper.h"
#include "ObjectHandler.h"
#include "Light.h"
#pragma once
class Render
{
private:
	void ClearBindings();
public:
	//Rendering Basics
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Texture2D* dsTexture = nullptr;
	ID3D11DepthStencilView* dsView = nullptr;
	D3D11_VIEWPORT viewport = {};
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;

	Render();
	~Render();

	//void StandardRender(ID3D11ShaderResourceView* textureSRV, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* vertexBuffer, ID3D11Buffer* matrixConstantBuffer, Parser* baseModel, ID3D11SamplerState* sampler, ID3D11ShaderResourceView* lightSRV, ID3D11Buffer* numberOfLightsBuffer, ID3D11Buffer* cameraPosConstantBuffer);
	void ShadowMap(ObjectHandler* object, LightHandler* lights, ID3D11VertexShader* vShaderShadow, ID3D11PixelShader* pShaderShadow, ID3D11Buffer* matrixConstantBuffer);
	void StandardRender(ObjectHandler* object, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer);
	//void CubeRender(ID3D11Buffer* vertexBuffer, ID3D11Buffer* currentLightConstBuff[4], ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cubeConstBuffer[6], ID3D11Buffer* cameraPosConstantBuffer, ID3D11Buffer* cubeVertexBuffer);
	//void CullingRender(ID3D11Buffer* matrixConstantBuffer, std::vector<int> marked, ID3D11Buffer* frustumVertexBuffer[50], ID3D11ShaderResourceView* frustumSRV[50], std::vector<Parser*>& frustumObjects);
	//void LODRender(ID3D11Buffer* vertexBuffer, ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer);
	//void ParticleRender(ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer);
};

