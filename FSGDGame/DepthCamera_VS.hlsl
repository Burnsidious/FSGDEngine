#ifndef DEPTH_CAMERA_VS_HLSL
#define DEPTH_CAMERA_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

DepthVertOut main( VERTEX_POS input )
{
	DepthVertOut output = (DepthVertOut)0;
	// SOLUTION_BEGIN
	output.position = mul(float4(input.position, 1.0), gMVP);

    output.depth = output.position.zw;
	// SOLUTION_END
	return output;
}

#endif //DEPTH_CAMERA_VS_HLSL