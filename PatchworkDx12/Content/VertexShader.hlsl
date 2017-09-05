
#include "CommonStructures.hlsli"


VertexToHull main(VertexData input)
{
	VertexToHull output;
	output.pos = input.pos;

	return output;
}