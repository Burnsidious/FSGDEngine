#ifndef TESSELATION_TEST_VS_HLSL
#define TESSELATION_TEST_VS_HLSL

#include "VertexLayouts.hlsli"

TESSELATION_TEST_VS_OUT main( VERTEX_POSNORMTEX input )
{
	TESSELATION_TEST_VS_OUT output;

	// Transfrom the position
	output.position = input.position;

	output.normal = float4(input.normal, 0);

	output.texCoord = input.texCoord;

	return output;
}

#endif //TESSELATION_TEST_VS_HLSL
