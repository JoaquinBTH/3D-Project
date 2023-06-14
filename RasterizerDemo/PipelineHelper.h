#pragma once

#include <array>

#include <cmath>

#include "Parser.h"

#include "Light.h"

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

struct CameraPos
{
	DirectX::XMFLOAT3 cameraPos;
	float filler = 0.0f;

	CameraPos(const DirectX::XMFLOAT3& camPos)
	{
		cameraPos = camPos;
	}
};

bool SetupPipeline(ID3D11Device * device, ID3D11Buffer *& vertexBuffer, ID3D11VertexShader *& vShader, ID3D11PixelShader *& pShader, ID3D11InputLayout *& inputLayout,
					ID3D11Buffer*& matrixConstantBuffer, Parser*& baseModel, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler, ID3D11Buffer*& cameraPosConstantBuffer);
