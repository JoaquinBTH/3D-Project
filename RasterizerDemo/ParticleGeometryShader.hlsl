struct GSOutput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

cbuffer cameraPosBuffer : register(b0)
{
	float3 cameraPosition;
}

cbuffer ConstantBuffer : register (b1)
{
	matrix world;
	matrix viewAndProjection;
}

[maxvertexcount(6)]
void main(
	point float4 input[1] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
	//Calculate normal using the camera position and world position.
	float3 normal = cameraPosition.xyz - input[0].xyz;
	normal.y = 0.0f;
	normal = normalize(normal);

	//Middle point of quad
	float2 uv = float2(0.5f, 0.5f);

	//Calculate the rightVector of the normal
	float3 rightVec = normalize(cross(normal, float3(0.0f, 1.0f, 0.0f)));
	rightVec = rightVec * 0.25f; //Half-width of quad
	float3 upVec = float3(0.0f, 0.25f, 0.0f); //Half-height of quad

	//Create the positions of each vertice in the quad
	float4 pos[4] = { float4((input[0].xyz - rightVec + upVec), input[0].w), float4((input[0].xyz + rightVec + upVec), input[0].w), 
						float4((input[0].xyz - rightVec - upVec), input[0].w), float4((input[0].xyz + rightVec - upVec), input[0].w)};

	GSOutput element;

	//Multiply each vertice with the viewAndProjection matrix.
	element.position = mul(viewAndProjection, pos[0]); //Upper left corner
	element.normal = normal.xyz;
	element.uv = float2(uv.x * 2, 1.0f) - float2(uv.x - 0.5f, uv.y + 0.5f);
	output.Append(element);

	element.position = mul(viewAndProjection, pos[1]); //Upper right corner
	element.normal = normal.xyz;
	element.uv = float2(uv.x * 2, 1.0f) - float2(uv.x + 0.5f, uv.y + 0.5f);
	output.Append(element);

	element.position = mul(viewAndProjection, pos[2]); //Lower left corner
	element.normal = normal.xyz;
	element.uv = float2(uv.x * 2, 1.0f) - float2(uv.x - 0.5f, uv.y - 0.5f);
	output.Append(element);

	element.position = mul(viewAndProjection, pos[3]); //Lower right corner
	element.normal = normal.xyz;
	element.uv = float2(uv.x * 2, 1.0f) - float2(uv.x + 0.5f, uv.y - 0.5f);
	output.Append(element);

	output.RestartStrip();
}