#include "VertexLayouts.hlsli"

float4 main(VERTEX_POSTEX input) : SV_POSITION
{
	return float4(input.position, 1);
}