struct Particle
{
	float3 pos;
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

StructuredBuffer<Particle> particles : register(t0);

VertexShaderOutput main(uint vertexID : SV_VertexID)
{
	Particle currentParticle = particles[vertexID];
	VertexShaderOutput output;
	output.position = mul(world, float4(currentParticle.pos, 1.0f));

	return output;
}