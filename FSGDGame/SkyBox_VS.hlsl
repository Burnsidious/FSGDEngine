#ifndef SKY_BOX_VS_HLSL
#define SKY_BOX_VS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

CubeVertOut main(VERTEX_POS input)
{
	CubeVertOut outVert;
	outVert.texCoord = float3(input.position.x, input.position.y, input.position.z);
	float3 pos = input.position + gCameraPos;
	outVert.position = mul(float4(pos, 1.0), gMVP);
	outVert.position.z = outVert.position.w;

	return outVert;
}

#endif //SKY_BOX_VS_HLSL