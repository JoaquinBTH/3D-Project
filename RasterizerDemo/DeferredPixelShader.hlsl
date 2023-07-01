Texture2DArray ambientTextures : register(t0);
Texture2DArray diffuseTextures : register(t1);
Texture2DArray specularTextures : register(t2);
SamplerState usedSampler;

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	float ns : NS;
	uint usedtexture : TEXTURE;
};

struct PixelShaderOutput
{
	float4 position : SV_Target0;
	float4 normal : SV_Target1;
	float4 ambient : SV_Target2;
	float4 diffuse : SV_Target3;
	float4 specular : SV_Target4;
	float4 shininess : SV_Target5;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;

	//Send the data to the G-Buffers
	output.position = input.worldPosition;
	output.normal = float4(input.normal, 0.0f);
	output.ambient = ambientTextures.Sample(usedSampler, float3(input.uv, input.usedtexture));
	output.diffuse = diffuseTextures.Sample(usedSampler, float3(input.uv, input.usedtexture));
	output.specular = specularTextures.Sample(usedSampler, float3(input.uv, input.usedtexture));
	output.shininess = float4(input.ns, 0.0f, 0.0f, 0.0f);

	return output;
}