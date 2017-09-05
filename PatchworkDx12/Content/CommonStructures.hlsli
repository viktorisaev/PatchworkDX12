// input

struct VertexData
{
	float3 pos : POSITION;
};

// vertex

struct VertexToHull
{
	float3 pos : POSITION;
};

// hull

struct HullToDomain
{
	float3 pos : POSITION;
};

// domain

struct DomainToPixel
{
	float4 pos : SV_POSITION;
	float3 color : COLOR;
};

// pixel





struct ConstantBufferPerObj
{
	row_major float4x4 wMat;
	matrix vpMat;
};


// number of control points to control the surface. We use 16-points patch.
#define NUM_CONTROL_POINTS 16
