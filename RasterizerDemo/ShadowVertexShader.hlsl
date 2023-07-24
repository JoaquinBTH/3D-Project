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
};

cbuffer ConstantBuffer : register (b0)
{
	matrix world;
	matrix viewAndProjection;
}

cbuffer Light : register(b1)
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 position;
	float range;
	float3 direction;
	float angle;
	float3 attenuation;
	uint isDirectional;
	matrix lightViewAndProjectionMatrix;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	output.position = mul(world, float4(input.position, 1.0f));
	output.position = mul(lightViewAndProjectionMatrix, output.position);

	return output;
}