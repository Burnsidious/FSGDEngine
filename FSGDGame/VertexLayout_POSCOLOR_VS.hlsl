#include "VertexLayouts.hlsli"

float4 main(VERTEX_POSCOLOR input) : SV_POSITION
{
	return float4(input.position.xyz, 1);
}