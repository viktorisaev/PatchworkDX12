#include "CommonStructures.hlsli"


float4 main(DomainToPixel input) : SV_TARGET
{
	float4 output;

	// lighting
	float3 light = normalize(float3(-1.0f, -1.0f, 1.0f));
	float lighting = 0.0f + dot(light, -input.norm);

	float3 refl = reflect(light, input.norm);
	float3 viewer = normalize(float3(0.0f, 0.0f, -7.0f));

	float3 reflex = pow(max(dot(refl, viewer), 0.0f), 28.0f);

	float3 color = float3(1.0f, 0.0f, 0.05f);
	float3 ambient = float3(0.1f, 0.1f, 0.1f);
	float3 diffuse = lighting * color + ambient;
	output = float4(diffuse + reflex, 1.0f);

//	return float4(input.norm, 1.0f);	// show normal
//	return float4(input.color, 1.0f);	// show color

	return output;
}