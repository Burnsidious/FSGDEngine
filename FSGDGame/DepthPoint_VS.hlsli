#ifndef DEPTH_POINT_BUMP_VS_HLSL
#define DEPTH_POINT_BUMP_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/LightBuffers.h"
#include "../EDRendererD3D/ConstantBuffers.h"

float4 DepthPoint( VERTEX_POS input, uniform unsigned int index ) : SV_POSITION
{
	return mul(float4(input.position, 1.0), gWorld);
}

#endif //DEPTH_POINT_BUMP_VS_HLSL