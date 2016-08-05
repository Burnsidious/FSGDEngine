#ifndef TWOD_PASS_THROUGH_WITH_POSITION_VS_HLSL
#define TWOD_PASS_THROUGH_WITH_POSITION_VS_HLSL

#include "VertexLayouts.hlsli"

TwoDVertexWithPositionOut main( VERTEX_POSTEX input)
{
	TwoDVertexWithPositionOut output;

	output.position.xyz = input.position;
	output.position.w = 1;
	
	output.pixelPosition = output.position;

	output.texCoord = input.texCoord;

	return output;
}

#endif //TWOD_PASS_THROUGH_WITH_POSITION_VS_HLSL