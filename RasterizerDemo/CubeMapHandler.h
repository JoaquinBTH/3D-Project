#pragma once
#include "ObjectHandler.h"

struct CubeFace
{
	XMFLOAT4X4 cubeViewAndProjectionMatrix;

	CubeFace(const XMVECTOR& lookAt, const XMVECTOR& position, const bool Y)
	{
		float fovRadians = (90.0f / 360.0f) * XM_2PI;
		XMMATRIX projection = XMMatrixPerspectiveFovLH(fovRadians, (1024.0f / 1024.0f), 0.1f, 1000.0f);
		XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX view = XMMatrixLookAtLH(position, lookAt, upVector);

		if (Y)
		{
			XMMATRIX rotationY = XMMatrixRotationY(XM_PIDIV2);
			view = rotationY * view;
		}

		XMMATRIX viewAndProjection = view * projection;
		XMStoreFloat4x4(&cubeViewAndProjectionMatrix, viewAndProjection);
	}
};

class CubeMapHandler
{
private:
	void CreateRTV(ID3D11Device* device, int index);
public:
	CubeMapHandler(ID3D11Device* device, const UINT WIDTH);
	~CubeMapHandler();

	ObjectHandler* cubeMapObject = nullptr; //Object to apply the cube mapping to in the end
	ID3D11Texture2D* cubeTexture = nullptr;
	ID3D11RenderTargetView* cubeRTV[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; //Render to each side of the cube
	ID3D11ShaderResourceView* cubeSRV = nullptr; //TextureCube SRV
	ID3D11Buffer* cubeConstBuff[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; //View and projection matrices for each face of the cube
	D3D11_VIEWPORT cubeport = {}; //New viewport because the texture that the cubemap samples from has to be 1:1 aspect ratio instead of 2:1 which is the window's aspect ratio

	ID3D11VertexShader* cubeVertexShader = nullptr; //BasicVertexShader but with constant buffer for the cubeConstBuff (Uses BasicPixelShader and apply cubeRTV as the RTV instead of swapchain)
	ID3D11PixelShader* cubePixelShader = nullptr; //Takes a TextureCube as its texture and outputs the reflection on the surface of the cube mapped object to the swapchain (Used in combination with BasicVertexShader)
};

