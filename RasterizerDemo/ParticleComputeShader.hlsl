struct Particle
{
	float3 pos;
};

RWStructuredBuffer<Particle> particles : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	Particle currentParticle = particles[DTid.x];
	currentParticle.pos = float3(currentParticle.pos.x, currentParticle.pos.y - 0.0005f, currentParticle.pos.z);
	if (currentParticle.pos.y < -5.0f)
	{
		currentParticle.pos.y = 5.0f; //Reset back to top
	}

	particles[DTid.x] = currentParticle;
}