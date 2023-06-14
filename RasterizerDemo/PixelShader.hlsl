Texture2DArray textures : register(t0);
SamplerState usedSampler;

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	uint usedtexture : TEXTURE;
};

struct PixelShaderOutput
{
	float4 swapchain : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
	//Define the material
	float4 material = textures.Sample(usedSampler, float3(input.uv, input.usedtexture));
	PixelShaderOutput output;

	output.swapchain = material;
	return output;
}