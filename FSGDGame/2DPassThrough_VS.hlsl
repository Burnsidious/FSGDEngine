#ifndef TWOD_PASS_THROUGH_VS_HLSL
#define TWOD_PASS_THROUGH_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

TwoDVertexOut main( VERTEX_POSTEX input)
{
	TwoDVertexOut output;

	output.position.xyz = input.position;
	output.position.w = 1;

	output.texCoord = input.texCoord * gTexCoordScale;

	return output;
}

#endif //TWOD_PASS_THROUGH_VS_HLSL