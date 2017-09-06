#include "CommonStructures.hlsli"


ConstantBufferPerObj constPerObject : register(b0);

struct PatchTransform
{
	row_major float4x4 transform;
};
StructuredBuffer<PatchTransform> patchTransforms : register(t0);


struct PatchColor
{
	float3 color;
};
StructuredBuffer<PatchColor> patchColors : register(t1);


struct PatchConstantData
{
	float edgeTessFactor[4] : SV_TessFactor;
	float insideTessFactor[2] : SV_InsideTessFactor;
};




float4 BernsteinBasis(float t)
{
	float invT = 1.0f - t;
	return float4(invT * invT * invT,	// (1-t)3
		3.0f * t * invT * invT,			// 3t(1-t)2
		3.0f * t * t * invT,			// 3t2(1-t)
		t * t * t);						// t3
}

float4 DBernsteinBasis(float t)
{
	float invT = 1.0f - t;

	return float4(-3 * invT * invT,		// -3(1-t)2
		3 * invT * invT - 6 * t * invT,	// 3(1-t)2 - 6t(1-t)
		6 * t * invT - 3 * t * t,		// 6t(1-t) - 3t2
		3 * t * t);						// 3t2
}

float3 evaluateBezier(const OutputPatch<HullToDomain, NUM_CONTROL_POINTS> bezpatch, float4 basisU, float4 basisV)
{
	float3 value = float3(0, 0, 0);
	value = basisV.x * (bezpatch[0].pos * basisU.x + bezpatch[1].pos * basisU.y + bezpatch[2].pos * basisU.z + bezpatch[3].pos * basisU.w);
	value += basisV.y * (bezpatch[4].pos * basisU.x + bezpatch[5].pos * basisU.y + bezpatch[6].pos * basisU.z + bezpatch[7].pos * basisU.w);
	value += basisV.z * (bezpatch[8].pos * basisU.x + bezpatch[9].pos * basisU.y + bezpatch[10].pos * basisU.z + bezpatch[11].pos * basisU.w);
	value += basisV.w * (bezpatch[12].pos * basisU.x + bezpatch[13].pos * basisU.y + bezpatch[14].pos * basisU.z + bezpatch[15].pos * basisU.w);

	return value;
}




[domain("quad")]
DomainToPixel main(PatchConstantData input, float2 domain : SV_DomainLocation, const OutputPatch<HullToDomain, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
	// Evaluate the basis functions at (u, v)
	float4 BasisU = BernsteinBasis(domain.x);
	float4 BasisV = BernsteinBasis(domain.y);
	float4 dBasisU = DBernsteinBasis(domain.x);
	float4 dBasisV = DBernsteinBasis(domain.y);

	// Evaluate the surface position for this vertex
	float3 localPos = evaluateBezier(patch, BasisU, BasisV);
	float3 tangent = evaluateBezier(patch, dBasisU, BasisV);
	float3 biTangent = evaluateBezier(patch, BasisU, dBasisV);
	float3 norm = normalize(cross(tangent, biTangent));

	float4x4 transform = patchTransforms[patchID].transform;
	float4 localPosTransformed = mul(float4(localPos, 1.0f), transform);

	DomainToPixel output;
	output.pos = mul(localPosTransformed, constPerObject.wMat);
	output.pos = mul(output.pos, constPerObject.vpMat);
	output.color = patchColors[patchID].color;
	norm = mul(norm, transform);
	output.norm = mul(norm, constPerObject.wMat);

	return output;
}