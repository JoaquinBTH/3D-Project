#include "DeferredHandler.h"

void DeferredHandler::InitializeGBuffer(ID3D11Device* device, const UINT WIDTH, const UINT HEIGHT, ID3D11Texture2D*& texture, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv)
{
	//Create the texture description
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = WIDTH;
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//Create the texture resource
	device->CreateTexture2D(&textureDesc, nullptr, &texture);

	//Create the render target view description
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	//Bind the texture to the render target view
	if (texture != NULL)
	{
		device->CreateRenderTargetView(texture, &rtvDesc, &rtv);
	}

	//Create the shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//Bind the texture to the shader resource view
	if (texture != NULL)
	{
		device->CreateShaderResourceView(texture, &srvDesc, &srv);
	}
}

DeferredHandler::DeferredHandler(ID3D11Device* device, const UINT WIDTH, const UINT HEIGHT)
{
	//Initialize all the GBuffers
	InitializeGBuffer(device, WIDTH, HEIGHT, positionTexture, positionRTV, positionSRV);
	InitializeGBuffer(device, WIDTH, HEIGHT, normalTexture, normalRTV, normalSRV);
	InitializeGBuffer(device, WIDTH, HEIGHT, ambientTexture, ambientRTV, ambientSRV);
	InitializeGBuffer(device, WIDTH, HEIGHT, diffuseTexture, diffuseRTV, diffuseSRV);
	InitializeGBuffer(device, WIDTH, HEIGHT, specularTexture, specularRTV, specularSRV);
	InitializeGBuffer(device, WIDTH, HEIGHT, shininessTexture, shininessRTV, shininessSRV);
}

DeferredHandler::~DeferredHandler()
{
	//Release all COM objects
	if (positionTexture) positionTexture->Release();
	if (normalTexture) normalTexture->Release();
	if (ambientTexture) ambientTexture->Release();
	if (diffuseTexture) diffuseTexture->Release();
	if (specularTexture) specularTexture->Release();
	if (shininessTexture) shininessTexture->Release();

	if (positionRTV) positionRTV->Release();
	if (normalRTV) normalRTV->Release();
	if (ambientRTV) ambientRTV->Release();
	if (diffuseRTV) diffuseRTV->Release();
	if (specularRTV) specularRTV->Release();
	if (shininessRTV) shininessRTV->Release();

	if (positionSRV) positionSRV->Release();
	if (normalSRV) normalSRV->Release();
	if (ambientSRV) ambientSRV->Release();
	if (diffuseSRV) diffuseSRV->Release();
	if (specularSRV) specularSRV->Release();
	if (shininessSRV) shininessSRV->Release();

	if (deferredVertexShader) deferredVertexShader->Release();
	if (deferredPixelShader) deferredPixelShader->Release();
	if (deferredComputeShader) deferredComputeShader->Release();
}
