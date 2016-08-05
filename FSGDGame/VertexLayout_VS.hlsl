#include "VertexLayouts.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

// This file consists of stub vertex shader methods to be used when creating InputLayout interfaces for validation

float4 VLPos(VERTEX_POS input) : SV_POSITION
{
	return float4(input.position, 1);
}

float4 VLPosColor(VERTEX_POSCOLOR input) : SV_POSITION
{
	return float4(input.position.xyz, 1);
}

float4 VLPosTex(VERTEX_POSTEX input) : SV_POSITION
{
	return float4(input.position, 1);
}

float4 VLPosNormTex(VERTEX_POSNORMTEX input) : SV_POSITION
{
	return float4(input.position, 1);
}

float4 VLPosNormTanTex(VERTEX_POSNORMTANTEX input) : SV_POSITION
{
	return float4(input.position, 1);
}
