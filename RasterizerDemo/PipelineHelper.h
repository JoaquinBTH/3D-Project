#pragma once

#include <array>
#include <d3d11.h>
#include <cmath>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>

using namespace DirectX;

struct MatrixConstantBuffer
{
	XMFLOAT4X4 worldMatrix;

	XMFLOAT4X4 viewAndProjectionMatrix;

	MatrixConstantBuffer(const XMMATRIX& wM, const XMMATRIX& vAPM)
	{
		XMStoreFloat4x4(&worldMatrix, wM);
		XMStoreFloat4x4(&viewAndProjectionMatrix, vAPM);
	}
};

bool SetupPipeline(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& matrixConstantBuffer, ID3D11SamplerState*& sampler);
