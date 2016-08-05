#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/LightBuffers.h"
#include "../EDRendererD3D/ConstantBuffers.h"

PointDepthVertOut main(VERTEX_POSNORMTEX input)
{
	PointDepthVertOut outVert;

	outVert.position = mul(float4(input.position, 1.0), gWorld);
	outVert.normalW = mul(float4(input.normal, 0.0), gWorld).xyz;
	outVert.texCoord = input.texCoord;

	return outVert;
}