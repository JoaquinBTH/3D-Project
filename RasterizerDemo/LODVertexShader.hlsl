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
	float distance : DISTANCE;
};

cbuffer ConstantBuffer : register (b0)
{
	matrix world;
	matrix viewAndProjection;
}

cbuffer MiddlePoint : register (b1)
{
	float3 middlePosition;
	float padding;
}

cbuffer cameraPosBuffer : register(b2)
{
	float3 cameraPosition;
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.position = float4(input.position, 1.0f);
	output.worldPosition = output.position;
	output.normal = input.normal;
	output.uv = input.uv;
	output.ns = input.ns;
	output.usedtexture = input.usedtexture;
	output.distance = distance(mul(world, float4(middlePosition, 1.0f)), (mul(world, float4(cameraPosition, 1.0f))));

	return output;
}