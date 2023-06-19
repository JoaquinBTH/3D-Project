#include "Light.h"
using namespace DirectX;

void LightHandler::AddShadowMap(ID3D11Device* device, Light light)
{
	//Remake the shadow map variables
	if ((shadowMapSRV != nullptr) || (shadowMapTexture != nullptr))
	{
		shadowMapSRV->Release();
		shadowMapTexture->Release();
	}
	for (int i = 0; i < lightSMV.size(); i++)
	{
		//Only shadowMapDSV because the individualLightConstantBuffer does not have to change when new lights are added
		if (lightSMV[i].shadowMapDSV != nullptr)
		{
			lightSMV[i].shadowMapDSV->Release();
		}
	}

	ShadowMapVariables SMV; //SMV to add to vector

	//Create constant buffer description
	D3D11_BUFFER_DESC constBuffDesc{};
	constBuffDesc.ByteWidth = sizeof(Light);
	constBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	constBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBuffDesc.CPUAccessFlags = 0;
	constBuffDesc.MiscFlags = 0;
	constBuffDesc.StructureByteStride = 0;

	//Create constant buffer subresource data
	D3D11_SUBRESOURCE_DATA constBuffData{};
	constBuffData.pSysMem = &light;
	constBuffData.SysMemPitch = 0;
	constBuffData.SysMemSlicePitch = 0;

	//Create the constant buffer
	device->CreateBuffer(&constBuffDesc, &constBuffData, &SMV.individualLightConstantBuffer);

	//Create 2D texture description
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = 1024;
	textureDesc.Height = 512;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = UINT(lightSMV.size() + 1);
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//Create 2D texture
	device->CreateTexture2D(&textureDesc, 0, &shadowMapTexture);

	//Create depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc{};
	DSVdesc.Flags = 0;
	DSVdesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DSVdesc.Texture2DArray.ArraySize = 1;
	DSVdesc.Texture2DArray.MipSlice = 0;

	if (shadowMapTexture != NULL)
	{
		//Re-create all the depth stencils for existing lights
		for (int i = 0; i < lightSMV.size(); i++)
		{
			DSVdesc.Texture2DArray.FirstArraySlice = i;
			device->CreateDepthStencilView(shadowMapTexture, &DSVdesc, &lightSMV[i].shadowMapDSV);
		}

		//Create the depth stencil for the new light
		DSVdesc.Texture2DArray.FirstArraySlice = UINT(lightSMV.size());
		device->CreateDepthStencilView(shadowMapTexture, &DSVdesc, &SMV.shadowMapDSV);

		//Add the new light shadow map variables to the list
		lightSMV.push_back(SMV);

		//Create shader resource view description
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc{};
		SRVdesc.Format = DXGI_FORMAT_R32_FLOAT;
		SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVdesc.Texture2DArray.ArraySize = UINT(lightSMV.size());
		SRVdesc.Texture2DArray.FirstArraySlice = 0;
		SRVdesc.Texture2DArray.MipLevels = 1;
		SRVdesc.Texture2DArray.MostDetailedMip = 0;

		//Create shader resource view to use all the depth stencils in shaders
		device->CreateShaderResourceView(shadowMapTexture, &SRVdesc, &shadowMapSRV);
	}
}

LightHandler::LightHandler()
{

}

LightHandler::~LightHandler()
{
	//Release Light COM objects
	if (lightBuffer != nullptr)
	{
		lightBuffer->Release();
	}
	if (lightSRV != nullptr)
	{
		lightSRV->Release();
	}
	if (numberOfLightsBuffer != nullptr)
	{
		numberOfLightsBuffer->Release();
	}
	lightList.clear();

	//Release Shadow Map COM objects
	if (shadowMapSRV != nullptr)
	{
		shadowMapSRV->Release();
	}
	if (shadowMapTexture != nullptr)
	{
		shadowMapTexture->Release();
	}

	for (int i = 0; i < lightSMV.size(); i++)
	{
		if (lightSMV[i].individualLightConstantBuffer != nullptr)
		{
			lightSMV[i].individualLightConstantBuffer->Release();
		}

		if (lightSMV[i].shadowMapDSV != nullptr)
		{
			lightSMV[i].shadowMapDSV->Release();
		}
	}
	lightSMV.clear();

}

void LightHandler::AddLight(ID3D11Device* device, const int isDirectional, const XMFLOAT4 ambient, const XMFLOAT4 diffuse, const XMFLOAT4 specular, const XMFLOAT3 direction, const float range, const XMFLOAT3 position, const float angle, const XMFLOAT3 attenuation)
{
	//Remake the lightBuffer, lightSRV, and numberOfLights
	if ((lightBuffer != nullptr) || (lightSRV != nullptr) || (numberOfLightsBuffer != nullptr))
	{
		lightBuffer->Release();
		lightSRV->Release();
		numberOfLightsBuffer->Release();
	}

	//View Matrix
	XMFLOAT3 upVector(0.0f, 1.0f, 0.0f);
	XMVECTOR eyePos = XMLoadFloat3(&position);
	XMVECTOR focusPos = XMVectorAdd(eyePos, XMLoadFloat3(&direction));
	XMVECTOR up = XMLoadFloat3(&upVector);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePos, focusPos, up);

	//Projection Matrix
	XMMATRIX projectionMatrix;
	if (isDirectional)
	{
		projectionMatrix = XMMatrixOrthographicLH(1024.0f, 512.0f, 0.1f, 1000.0f);
	}
	else
	{
		constexpr float fovRadians = XMConvertToRadians(90.0f); //constexpr because it's computed at compile time.
		projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, 2.0f, 0.1f, 1000.0f); //Aspect ratio = (1024.0f / 512.0f) = 2.0f
	}

	//ViewAndProjection Matrix
	XMMATRIX viewAndProjectionMatrix = viewMatrix * projectionMatrix;

	//Set up the light
	Light light;
	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;
	light.position = position;
	light.range = range;
	light.direction = direction;
	light.angle = XMConvertToRadians(angle);
	light.attenuation = attenuation;
	light.isDirectional = isDirectional;
	XMStoreFloat4x4(&light.lightViewAndProjectionMatrix, viewAndProjectionMatrix);

	//Add the new light to the list of lights
	this->lightList.push_back(light);

	//Create the buffer description for the light
	D3D11_BUFFER_DESC lightDesc{};
	lightDesc.ByteWidth = UINT(sizeof(Light) * lightList.size());
	lightDesc.Usage = D3D11_USAGE_DEFAULT;
	lightDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	lightDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	lightDesc.StructureByteStride = sizeof(Light);

	//Initialize data for the buffer (Mapped resource could be used instead if usage is dynamic)
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = lightList.data();
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	//Create the shader resource view description for the light
	D3D11_SHADER_RESOURCE_VIEW_DESC lightSRVDesc{};
	lightSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	lightSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	lightSRVDesc.Buffer.FirstElement = 0;
	lightSRVDesc.Buffer.NumElements = UINT(lightList.size());
	lightSRVDesc.Buffer.ElementOffset = 0;

	//Create the buffer and shader resource view
	device->CreateBuffer(&lightDesc, &initialData, &this->lightBuffer);
	if (this->lightBuffer != NULL)
	{
		device->CreateShaderResourceView(this->lightBuffer, &lightSRVDesc, &this->lightSRV);
	}

	AddShadowMap(device, light);

	NumberOfLights num;
	num.nrOfLights = int(lightList.size());

	//Create constant buffer description
	D3D11_BUFFER_DESC constBuffDesc{};
	constBuffDesc.ByteWidth = sizeof(NumberOfLights);
	constBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	constBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBuffDesc.CPUAccessFlags = 0;
	constBuffDesc.MiscFlags = 0;
	constBuffDesc.StructureByteStride = 0;

	//Create constant buffer subresource data
	D3D11_SUBRESOURCE_DATA constBuffData{};
	constBuffData.pSysMem = &num;
	constBuffData.SysMemPitch = 0;
	constBuffData.SysMemSlicePitch = 0;

	device->CreateBuffer(&constBuffDesc, &constBuffData, &this->numberOfLightsBuffer);
}
