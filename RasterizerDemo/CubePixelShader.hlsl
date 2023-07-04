TextureCube cubeTexture : register(t0);
SamplerState usedSampler;

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
	uint usedtexture : TEXTURE;
};

struct PixelShaderOutput
{
	float4 swapChain : SV_Target;
};

cbuffer cameraPosBuffer : register(b0)
{
	float3 cameraPosition;
}

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;
	float3 viewVec = input.worldPosition.xyz - cameraPosition;
	float3 reflection = reflect(viewVec, normalize(input.normal));

	reflection = normalize(reflection);

	output.swapChain = cubeTexture.Sample(usedSampler, reflection);

	return output;
}