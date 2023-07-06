//#include "PipelineHelper.h"
#include "ObjectHandler.h"
#include "Light.h"
#include "DeferredHandler.h"
#include "LODHandler.h"
#include "CubeMapHandler.h"
#include "OctreeHandler.h"
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
	ID3D11UnorderedAccessView* uav = nullptr;

	Render();
	~Render();

	void ShadowMap(ObjectHandler* object, LightHandler* lights, ID3D11VertexShader* vShaderShadow, ID3D11PixelShader* pShaderShadow, ID3D11Buffer* matrixConstantBuffer);
	void StandardRender(ObjectHandler* object, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer);
	void DeferredRender(ObjectHandler* object, DeferredHandler* deferred, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer);
	void LODRender(ObjectHandler* object, LODHandler* LOD, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer);
	void CubeRender(ObjectHandler* object, CubeMapHandler* cubeMap, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11Buffer* matrixConstantBuffer, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* cameraPosConstantBuffer);
	void CullingRender(OctreeHandler* octree, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, ID3D11SamplerState* sampler, LightHandler* lights, ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer);
	//void ParticleRender(ID3D11Buffer* matrixConstantBuffer, ID3D11Buffer* cameraPosConstantBuffer);
};

