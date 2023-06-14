#include "Light.h"
using namespace DirectX;

Light::Light(const XMFLOAT3& lC, const float lS, const XMFLOAT3& lP, const int dL)
{
	lightColor = lC;
	lightStrength = lS;
	lightPosition = lP;
	directionalLight = dL;

	float fovRadians = (90.0f / 360.0f) * XM_2PI;
	XMMATRIX projection;

	if (directionalLight == 1)
	{
		projection = XMMatrixOrthographicLH(1024.0f, 512.0f, 0.1f, 1000.0f);
	}
	else
	{
		projection = XMMatrixPerspectiveFovLH(fovRadians, (1024.0f / 512.0f), 0.1f, 1000.0f);
	}
	XMVECTOR position = XMLoadFloat3(&lP);
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(position, lookAt, upVector);
	XMMATRIX viewAndProjection = view * projection;
	XMStoreFloat4x4(&lightViewAndProjectionMatrix, viewAndProjection);
}

void Light::SetPosition(float x, float y, float z)
{
	lightPosition = XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 Light::getLightColor()
{
	return this->lightColor;
}

float Light::getLightStrength()
{
	return this->lightStrength;
}

DirectX::XMFLOAT3 Light::getLightPosition()
{
	return this->lightPosition;
}

int Light::getDirectionLight()
{
	return this->directionalLight;
}

DirectX::XMFLOAT4X4 Light::getLightViewAndProjectionMatrix()
{
	return this->lightViewAndProjectionMatrix;
}
