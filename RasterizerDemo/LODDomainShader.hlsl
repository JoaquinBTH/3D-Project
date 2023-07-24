Texture2D heightMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState usedSampler;

struct DS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
	uint usedtexture : TEXTURE;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
	uint usedtexture : TEXTURE;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

cbuffer ConstantBuffer : register (b0)
{
	matrix world;
	matrix viewAndProjection;
}

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	//Interpolate the data from the primitive to get the values of the tessellated points
	Output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;
	Output.worldPosition = patch[0].worldPosition * domain.x + patch[1].worldPosition * domain.y + patch[2].worldPosition * domain.z;
	Output.normal = patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;
	Output.uv = patch[0].uv * domain.x + patch[1].uv * domain.y + patch[2].uv * domain.z;
	Output.ns = patch[0].ns;
	Output.usedtexture = patch[0].usedtexture;
	
	float3 baseNormal = Output.normal;

	//Apply displacement based on heightMap and normal mapping based on normalMap

	//Calculate the tangent and bitangent
	float3 edge1 = patch[1].position.xyz - patch[0].position.xyz;
	float3 edge2 = patch[2].position.xyz - patch[0].position.xyz;
	float2 deltaUV1 = patch[1].uv - patch[0].uv;
	float2 deltaUV2 = patch[2].uv - patch[0].uv;

	float det = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	float3 tangent = normalize((deltaUV2.y * edge1 - deltaUV1.y * edge2) * det);
	float3 bitangent = normalize((-deltaUV2.x * edge1 + deltaUV1.x * edge2) * det);

	//Create the TBN matrix and get the new normal
	float3 normalMapSample = normalMap.SampleLevel(usedSampler, Output.uv, 0).xyz * 2.0f - 1.0f; //Convert to [-1, 1] range
	float3x3 TBN = float3x3(tangent, bitangent, Output.normal);
	Output.normal = normalize(mul(normalMapSample, TBN));

	//Displace the position based on the normal
	float displacementAmount = (heightMap.SampleLevel(usedSampler, Output.uv, 0).r * 2.0f - 1.0f); //Height
	Output.position.xyz += baseNormal * (displacementAmount * 0.5f);

	//Transform the displaced position into clip space
	Output.position = mul(world, Output.position);
	Output.worldPosition = Output.position;
	Output.position = mul(viewAndProjection, Output.position);

	//Check if any tessllation is currently applied to the object
	if (input.InsideTessFactor == 1.0f)
	{
		//Base normal
		Output.normal = mul(world, float4(baseNormal, 1.0f)).xyz;
	}
	else
	{
		//Normal mapped
		Output.normal = mul(world, float4(Output.normal, 1.0f)).xyz;
	}

	return Output;
}