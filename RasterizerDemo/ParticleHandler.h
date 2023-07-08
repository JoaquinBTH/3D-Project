#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

using namespace DirectX;

struct Particle
{
	XMFLOAT3 position;

	Particle(XMFLOAT3 pos)
	{
		this->position = pos;
	}
};

class ParticleHandler
{
private:
	void LoadTexture(ID3D11Device* device);
public:

	ParticleHandler(ID3D11Device* device, const int nrOfParticles, XMFLOAT3 startPos = XMFLOAT3(0.0f, 5.0f, 0.0f));
	~ParticleHandler();

	ID3D11VertexShader* particleVertexShader = nullptr;
	ID3D11GeometryShader* particleGeometryShader = nullptr;
	ID3D11ComputeShader* particleComputeShader = nullptr;
	ID3D11PixelShader* particlePixelShader = nullptr;

	ID3D11Texture2D* particleTexture = nullptr;
	ID3D11ShaderResourceView* particleSRV = nullptr;

	ID3D11Buffer* particleBuffer = nullptr;
	ID3D11InputLayout* particleInputLayout = nullptr;
	ID3D11ShaderResourceView* particleStructuredSRV = nullptr;
	ID3D11UnorderedAccessView* particleStructuredUAV = nullptr;

	std::vector<Particle> particles;
};

