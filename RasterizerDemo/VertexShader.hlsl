struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	uint usedtexture : TEXTURE;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	uint usedtexture : TEXTURE;
};

cbuffer ConstantBuffer : register (b0)
{
	matrix world;
	matrix viewAndProjection;
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

	//Model values
	output.position = mul(world, float4(input.position, 1.0f));
	output.position = mul(viewAndProjection, output.position);
	output.uv = input.uv;
	output.usedtexture = input.usedtexture;

	return output;
}