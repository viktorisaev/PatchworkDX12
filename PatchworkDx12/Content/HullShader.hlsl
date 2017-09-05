#include "CommonStructures.hlsli"


struct PatchTesselationFactors
{
	int edge;
	int inside;
};
PatchTesselationFactors tessFactors : register(b0);


struct PatchConstantData
{
	float edgeTessFactor[4] : SV_TessFactor;
	float insideTessFactor[2] : SV_InsideTessFactor;
};


PatchConstantData calculatePatchConstants()
{
	PatchConstantData output;

	output.edgeTessFactor[0] = tessFactors.edge;
	output.edgeTessFactor[1] = tessFactors.edge;
	output.edgeTessFactor[2] = tessFactors.edge;
	output.edgeTessFactor[3] = tessFactors.edge;
	output.insideTessFactor[0] = tessFactors.inside;
	output.insideTessFactor[1] = tessFactors.inside;

	return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("calculatePatchConstants")]
HullToDomain main(InputPatch<VertexToHull, NUM_CONTROL_POINTS> input, uint i : SV_OutputControlPointID)
{
	HullToDomain output;
	output.pos = input[i].pos;

	return output;
}