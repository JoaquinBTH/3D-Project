#include <d3d11.h>
#include <DirectXMath.h>

#pragma once
class Light
{
private:
	DirectX::XMFLOAT3 lightColor;
	float lightStrength;

	DirectX::XMFLOAT3 lightPosition;
	int directionalLight;

	DirectX::XMFLOAT4X4 lightViewAndProjectionMatrix;

public:
	Light(const DirectX::XMFLOAT3& lC, const float lS, const DirectX::XMFLOAT3& lP, const int dL);

	void SetPosition(float x, float y, float z);

	DirectX::XMFLOAT3 getLightColor();
	float getLightStrength();
	DirectX::XMFLOAT3 getLightPosition();
	int getDirectionLight();
	DirectX::XMFLOAT4X4 getLightViewAndProjectionMatrix();
};

