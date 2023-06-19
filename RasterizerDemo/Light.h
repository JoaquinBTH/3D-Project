#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

#pragma once
class LightHandler
{
private:
	struct Light 
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT3 position;
		float range;
		XMFLOAT3 direction;
		float angle;
		XMFLOAT3 attenuation;
		int isDirectional;
		XMFLOAT4X4 lightViewAndProjectionMatrix;
	};

	struct NumberOfLights
	{
		int nrOfLights;
		XMFLOAT3 padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	};

	struct ShadowMapVariables
	{
		ID3D11Buffer* individualLightConstantBuffer = nullptr;
		ID3D11DepthStencilView* shadowMapDSV = nullptr;
	};

	std::vector<Light> lightList; //List of all lights
	ID3D11Buffer* lightBuffer = nullptr; //Structured buffer for the lights

	ID3D11Texture2D* shadowMapTexture = nullptr;
	void AddShadowMap(ID3D11Device* device, Light light);

public:
	ID3D11ShaderResourceView* lightSRV = nullptr; //Shader resource to use the structured buffer
	ID3D11ShaderResourceView* shadowMapSRV = nullptr; //Shader resource to use the generated shadow maps
	std::vector<ShadowMapVariables> lightSMV; //List of variables needed for shadow mapping
	ID3D11Buffer* numberOfLightsBuffer = nullptr;

	LightHandler();

	~LightHandler();

	void AddLight(ID3D11Device* device, const int isDirectional = 0, const XMFLOAT4 ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), const XMFLOAT4 diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				const XMFLOAT4 specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), const XMFLOAT3 direction = XMFLOAT3(0.0f, 0.0f, 1.0f), const float range = 100.0f,
				const XMFLOAT3 position = XMFLOAT3(0.0f, 5.0f, -5.0f), const float angle = 45.0f, const XMFLOAT3 attenuation = XMFLOAT3(1.0f, 0.01f, 0.01f));
};

