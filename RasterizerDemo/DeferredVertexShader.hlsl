struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
	uint usedtexture : TEXTURE;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
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

	output.worldPosition = mul(world, float4(input.position, 1.0f));
	output.position = mul(viewAndProjection, output.worldPosition);
	output.normal = normalize(mul(world, float4(input.normal, 0.0f)).xyz);
	output.uv = input.uv;
	output.ns = input.ns;
	output.usedtexture = input.usedtexture;

	return output;
}