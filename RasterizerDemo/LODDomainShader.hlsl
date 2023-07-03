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
	
	//Apply displacement based on heightMap and normalMap

	//Get the height from the heightMap
	float height = (heightMap.SampleLevel(usedSampler, Output.uv, 0).r * 2.0f - 1.0f) / 0.5f;

	//Calculate the normal and displace the position based on that normal
	Output.normal = normalize(mul(world, float4(Output.normal, 0.0f)).xyz);
	float3 displacedPosition = Output.position.xyz + Output.normal * (height * 0.5f);
	Output.position = float4(displacedPosition, 1.0f);

	//Transform the displaced position into clip space
	Output.position = mul(world, Output.position);
	Output.worldPosition = Output.position;
	Output.position = mul(viewAndProjection, Output.position);

	//Calculate the tangent and bitangent using the edge positions and UV coordinates of the patches.
	float3 edge1 = patch[1].position.xyz - patch[0].position.xyz;
	float3 edge2 = patch[2].position.xyz - patch[0].position.xyz;
	float2 deltaUV1 = patch[1].uv - patch[0].uv;
	float2 deltaUV2 = patch[2].uv - patch[0].uv;

	//Determinant of the UV coordinate differences.
	float det = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

	float3 tangent = (deltaUV2.y * edge1 - deltaUV1.y * edge2) / det;
	float3 bitangent = (deltaUV1.x * edge2 - deltaUV2.x * edge1) / det;

	//Construct the tangent-bitangent-normal matrix
	float3x3 TBN = float3x3(tangent, bitangent, Output.normal);

	//Sample the normal map with in the ranges of -1 to 1 and get the worldSpaceNormal after displacement is done
	float3 tangentSpaceNormal = normalMap.SampleLevel(usedSampler, Output.uv, 0).xyz * 2.0f - 1.0f;
	float3 worldSpaceNormal = normalize(mul(TBN, tangentSpaceNormal));

	//If there is tessellation, then we change the normal. Otherwise just pass along the current normal.
	if (input.InsideTessFactor != 1.0f)
	{
		Output.normal = worldSpaceNormal;
	}

	return Output;
}