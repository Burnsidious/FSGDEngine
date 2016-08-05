#ifndef LIT_TEXTURED_VS_HLSL
#define LIT_TEXTURED_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"
#include "ForwardLightingSupport.hlsli"

ForwardRenderVertexOut main( VERTEX_POSNORMTEX input )
{
	ForwardRenderVertexOut outVert;

	// Transfrom the position
	outVert.position = mul(float4(input.position, 1.0), gMVP);
	outVert.screenPos = outVert.position;
	outVert.worldPos = mul(float4(input.position, 1.0), gWorld);

	// Transform the normal, without translation
	outVert.normal = mul(float4(input.normal, 0.0), gWorld);

	// Pass the texCoord
	outVert.texCoord = input.texCoord;

	return outVert;
}

#endif //LIT_TEXTURED_VS_HLSL
