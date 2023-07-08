Texture2D particleTexture : register(t0);
SamplerState usedSampler;

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PixelShaderOutput
{
	float4 swapChain : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;

	output.swapChain = particleTexture.Sample(usedSampler, input.uv);
	return output;
}