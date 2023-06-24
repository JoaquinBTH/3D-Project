struct PixelShaderInput
{
	float4 position : SV_POSITION;
};

struct PixelShaderOutput
{
	float depth : SV_Depth;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;
	output.depth = input.position.z;
	return output;
}