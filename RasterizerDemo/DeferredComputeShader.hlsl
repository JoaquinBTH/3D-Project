RWTexture2D<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> normalGBuffer : register(t1);
Texture2D<float4> ambientGBuffer : register(t2);
Texture2D<float4> diffuseGBuffer : register(t3);
Texture2D<float4> specularGBuffer : register(t4);
Texture2D<float4> shininessGBuffer : register(t5);
Texture2DArray<float4> depthMap : register(t6);
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

StructuredBuffer<Light> lights : register (t7);

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

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//Get the value for the current pixel to be processed
	float4 position = positionGBuffer[DTid.xy].xyzw;
	float3 normal = normalGBuffer[DTid.xy].xyz;
	float4 ambientMap = ambientGBuffer[DTid.xy].xyzw;
	float4 diffuseMap = diffuseGBuffer[DTid.xy].xyzw;
	float4 specularMap = specularGBuffer[DTid.xy].xyzw;
	float shininess = shininessGBuffer[DTid.xy].x;


	float4 phongColor = float4(0.0f, 0.0f, 0.0f, 1.0f); //Define the variable for the phong reflection lighting

	float shadowFactor = 1.0f;

	//Caclulate the phong reflection lighting per light
	for (uint i = 0; i < nrOfLights; i++)
	{
		Light light = lights[i];

		//Get the pixel's position from the light's perspective and transform it into screenSpace
		float4 positionInLight = mul(light.lightViewAndProjectionMatrix, position);
		positionInLight = positionInLight * float4(0.5f, -0.5f, 1.0f, 1.0f) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * positionInLight.w);
		float2 shadowTexCoord = positionInLight.xy / positionInLight.w;

		//Sample the depthValue at the screenspace position
		float depthValue = depthMap.Load(int4(shadowTexCoord.x * 1024, shadowTexCoord.y * 512, i, 0)).r;
		float linearDepth = positionInLight.z / positionInLight.w;
		float bias = 0.0015f; //Deciding shadow factor if depths are too similar due to rounding errors.

		float3 surfaceNormal = normalize(normal); //Normal of the surface

		float3 ambient = light.ambient.rgb; //Light's ambient component

		float3 viewDirection = normalize(cameraPosition - position.xyz); //Direction from surface to camera

		if (!light.isDirectional)
		{
			float spotlightAttenuation = 0.0f; //Light intensity depending on if the surface is inside the spotlight or not
			float3 lightDirection = normalize(light.position - position.xyz); //Surface to light position
			float spotlightCosine = dot(-lightDirection, light.direction); //Cosine of angle between lightDirection and spot light's direction 

			bool isWithinSpotlight = spotlightCosine >= cos(light.angle); //Is the surface point within the spot light's cone?

			if (isWithinSpotlight)
			{
				spotlightAttenuation = 1.0f;
				float3 diffuseLight = saturate(dot(surfaceNormal, lightDirection)) * light.diffuse.rgb; //Diffuse light component
				float3 lightReflect = reflect(-lightDirection, surfaceNormal); //Direction of the reflected light 
				float specularIntensity = pow(saturate(dot(lightReflect, viewDirection)), shininess); //Intensity of the specular light
				float3 specularLight = specularIntensity * light.specular.rgb; //Specular light component
				float3 vecToLight = light.position - position.xyz; //Vector from the surface to the light
				float distance = length(vecToLight); //Distance of the aforementioned vector
				float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance); //Intensity of light (constant, linear, quadratic) 

				phongColor.rgb += ambient * ambientMap.rgb + spotlightAttenuation * attenuation * (diffuseLight * diffuseMap.rgb + specularLight * specularMap.rgb);

				//Compare the depth value with the pixel's current depth
				if (linearDepth - bias > depthValue)
				{
					//In Shadow
					shadowFactor *= 0.5f;
				}
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
				float specularIntensity = pow(saturate(dot(lightReflect, viewDirection)), shininess); //Intensity of the specular light
				float3 specularLight = specularIntensity * light.specular.rgb; //Specular light component

				phongColor.rgb += ambient * ambientMap.rgb + diffuseLight * diffuseMap.rgb + specularLight * specularMap.rgb;

				//Compare the depth value with the pixel's current depth
				if (linearDepth - bias > depthValue)
				{
					//In Shadow
					shadowFactor *= 0.5f;
				}
			}
		}
	}

	backBufferUAV[DTid.xy] = phongColor * shadowFactor;
}