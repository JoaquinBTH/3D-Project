struct VS_CONTROL_POINT_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
	uint usedtexture : TEXTURE;
	float distance : DISTANCE;
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

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	//Tessellation calculation variables
	float minDistance = 5.0f;
	float maxDistance = 3.0f;
	float maxTessellationFactor = 64.0f;

	//Calculate the tessellation factor to apply to the object based on distance
	float normalizedDistance = saturate((ip[0].distance - minDistance) / (maxDistance - minDistance));
	Output.EdgeTessFactor[0] =
		Output.EdgeTessFactor[1] =
		Output.EdgeTessFactor[2] =
		Output.InsideTessFactor = lerp(1.0f, maxTessellationFactor, normalizedDistance);

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	Output.position = ip[i].position;
	Output.worldPosition = ip[i].worldPosition;
	Output.normal = ip[i].normal;
	Output.uv = ip[i].uv;
	Output.ns = ip[i].ns;
	Output.usedtexture = ip[i].usedtexture;

	return Output;
}
