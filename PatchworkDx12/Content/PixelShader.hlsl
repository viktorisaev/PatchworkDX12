#include "CommonStructures.hlsli"


float4 main(DomainToPixel input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}