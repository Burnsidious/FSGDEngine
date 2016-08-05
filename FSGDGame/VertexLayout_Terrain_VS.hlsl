#include "VertexLayouts.hlsli"

float4 main(VERTEX_TERRAIN input) : SV_POSITION
{
	return float4(input.position, 0, 1);
}