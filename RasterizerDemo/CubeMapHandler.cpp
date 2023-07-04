#include "CubeMapHandler.h"

void CubeMapHandler::CreateRTV(ID3D11Device* device, int index)
{
	D3D11_RENDER_TARGET_VIEW_DESC RTVdesc{};
	RTVdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RTVdesc.Texture2DArray.ArraySize = 1;
	RTVdesc.Texture2DArray.FirstArraySlice = index;
	RTVdesc.Texture2DArray.MipSlice = 0;

	if (cubeTexture != NULL)
	{
		device->CreateRenderTargetView(cubeTexture, &RTVdesc, &cubeRTV[index]);
	}
}

CubeMapHandler::CubeMapHandler(ID3D11Device* device, const UINT WIDTH)
{
	//Create the object and the viewAndProjection matrices for each face
	cubeMapObject = new ObjectHandler();
	cubeMapObject->LoadObject(device, "Models/SmoothShadeSphere.obj");

	//Define the lookAt vectors and set the position
	XMVECTOR posX, negX, posY, negY, posZ, negZ, position;
	XMFLOAT3 objPos = cubeMapObject->getMiddlePoint();
	position = XMVectorSet(objPos.x, objPos.y, objPos.z, 0.0f);

	//Set the lookAt vectors
	posX = XMVectorSet(1000.0f, 0.0f, 0.0f, 0.0f);
	negX = XMVectorSet(-1000.0f, 0.0f, 0.0f, 0.0f);
	posY = XMVectorSet(0.0f, 1000.0f, 0.0f, 0.0f);
	negY = XMVectorSet(0.0f, -1000.0f, 0.0f, 0.0f);
	posZ = XMVectorSet(0.0f, 0.0f, 1000.0f, 0.0f);
	negZ = XMVectorSet(0.0f, 0.0f, -1000.0f, 0.0f);

	//Create the constant buffers for the matrices
	CubeFace faces[6] =
	{
		{posX, position, 0}, {negX, position, 0}, {posY, position, 1}, {negY, position, 1}, {posZ, position, 0}, {negZ, position, 0}
	};

	D3D11_BUFFER_DESC buffDesc{};
	buffDesc.ByteWidth = sizeof(CubeFace);
	buffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA buffData{};
	for (int i = 0; i < 6; i++)
	{
		buffData.pSysMem = &faces[i];
		buffData.SysMemPitch = 0;
		buffData.SysMemSlicePitch = 0;

		device->CreateBuffer(&buffDesc, &buffData, &cubeConstBuff[i]);
	}

	//Create the texture
	D3D11_TEXTURE2D_DESC cubeDesc{};
	cubeDesc.Width = 1024;
	cubeDesc.Height = 1024;
	cubeDesc.MipLevels = 1;
	cubeDesc.ArraySize = 6;
	cubeDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	cubeDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeDesc.CPUAccessFlags = 0;
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	device->CreateTexture2D(&cubeDesc, nullptr, &cubeTexture);

	//Create the RTVs
	for (int i = 0; i < 6; i++)
	{
		this->CreateRTV(device, i);
	}

	//Create the SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVdesc{};
	SRVdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SRVdesc.Texture2DArray.ArraySize = 6;
	SRVdesc.Texture2DArray.FirstArraySlice = 0;
	SRVdesc.Texture2DArray.MipLevels = 1;
	SRVdesc.Texture2DArray.MostDetailedMip = 0;

	if (cubeTexture != NULL)
	{
		device->CreateShaderResourceView(cubeTexture, &SRVdesc, &cubeSRV);
	}

	//Set the cubeport
	cubeport.TopLeftX = 0;
	cubeport.TopLeftY = 0;
	cubeport.Width = static_cast<float>(WIDTH);
	cubeport.Height = static_cast<float>(WIDTH);
	cubeport.MinDepth = 0;
	cubeport.MaxDepth = 1;
}

CubeMapHandler::~CubeMapHandler()
{
	if (cubeMapObject) delete cubeMapObject;
	if (cubeTexture) cubeTexture->Release();
	for (int i = 0; i < 6; i++)
	{
		if (cubeRTV[i]) cubeRTV[i]->Release();
		if (cubeConstBuff[i]) cubeConstBuff[i]->Release();
	}
	if (cubeSRV) cubeSRV->Release();
	if (cubeVertexShader) cubeVertexShader->Release();
	if (cubePixelShader) cubePixelShader->Release();
}
