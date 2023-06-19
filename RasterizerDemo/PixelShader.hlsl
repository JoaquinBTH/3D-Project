Texture2DArray textures : register(t0);
SamplerState usedSampler;

struct Light
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

StructuredBuffer<Light> lights : register (t1);

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD;
	float3 normal : NORMAL;
	float2 uv : UV;
	uint usedtexture : TEXTURE;
};

struct PixelShaderOutput
{
	float4 swapchain : SV_Target0;
};

cbuffer ConstantBuffer : register (b0)
{
	matrix world;
	matrix viewAndProjection;
}

cbuffer ConstantBuffer : register (b1)
{
	uint nrOfLights;
	float3 padding;
}

cbuffer cameraPosBuffer : register(b2)
{
	float3 cameraPosition;
}

PixelShaderOutput main(PixelShaderInput input)
{
	//Define the material
	float4 material = textures.Sample(usedSampler, float3(input.uv, input.usedtexture));
	PixelShaderOutput output;

	float4 phongColor = float4(0.0f, 0.0f, 0.0f, 1.0f); //Define the variable for the phong reflection lighting
	//Caclulate the phong reflection lighting per light
	for (uint i = 0; i < nrOfLights; i++)
	{
		Light light = lights[i];
		//Remove the comments from these if you want the light to rotate with the scene
		//light.position = mul(world, float4(light.position, 1.0f)).xyz;
		//light.direction = normalize(mul(world, float4(light.direction, 0.0f)).xyz);

		float3 surfaceNormal = normalize(input.normal); //Normal of the surface

		float3 ambient = light.ambient.rgb; //Light's ambient component

		float3 viewDirection = normalize(cameraPosition - input.worldPosition.xyz); //Direction from surface to camera

		if (!light.isDirectional)
		{
			float spotlightAttenuation = 0.0f; //Light intensity depending on if the surface is inside the spotlight or not
			float3 lightDirection = normalize(light.position - input.worldPosition.xyz); //Surface to light position
			float spotlightCosine = dot(-lightDirection, light.direction); //Cosine of angle between lightDirection and spot light's direction 
		
			bool isWithinSpotlight = spotlightCosine >= cos(light.angle); //Is the surface point within the spot light's cone?
		 
		  if(isWithinSpotlight)
			{
				spotlightAttenuation = 1.0f;
				float3 diffuseLight = saturate(dot(surfaceNormal, lightDirection)) * light.diffuse.rgb; //Diffuse light component
				float3 lightReflect = reflect(-lightDirection, surfaceNormal); //Direction of the reflected light 
				float specularIntensity = pow(saturate(dot(lightReflect, viewDirection)), 32.0f); //Intensity of the specular light
				float3 specularLight = specularIntensity * light.specular.rgb; //Specular light component
				float3 vecToLight = light.position - input.worldPosition.xyz; //Vector from the surface to the light
				float distance = length(vecToLight); //Distance of the aforementioned vector
				float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance); //Intensity of light (constant, linear, quadratic) 

				phongColor.rgb += ambient + spotlightAttenuation * attenuation * (diffuseLight + specularLight);
		  }
		}
		else if (light.isDirectional)
		{
			float3 lightDirection = normalize(-light.direction); //Because the light is so far away the direction from the surface to the light is just the inverse of the light's direction
			float facingLight = dot(surfaceNormal, lightDirection); //Check if the surfaceNormal is facing the light or not
			if (facingLight > 0.0f)
			{
				float3 diffuseLight = saturate(dot(surfaceNormal, lightDirection)) * light.diffuse.rgb; //Diffuse light component
				float3 lightReflect = reflect(-lightDirection, surfaceNormal); //Direction of the reflected light
				float specularIntensity = pow(saturate(dot(lightReflect, viewDirection)), 32.0f); //Intensity of the specular light
				float3 specularLight = specularIntensity * light.specular.rgb; //Specular light component
		
				phongColor.rgb += ambient + diffuseLight + specularLight;
			}
		}
	}

	output.swapchain = material * phongColor;
	return output;
}