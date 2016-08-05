#ifndef LINE_WIDTH_GS_HLSL
#define LINE_WIDTH_GS_HLSL

#include "VertexLayouts.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

[maxvertexcount(6)]
void main(
	line PosColorOut input[2], 
	inout TriangleStream< PosColorOut > output
)
{
	PosColorOut p2, p3;

	// Do the work
	float4 p0 = input[0].position;
	p0.xyz /= p0.w;
	float4 p1 = input[1].position;
	p1.xyz /= p1.w;

	// remove normalize ?
	float3 tangent = p1.xyz - p0.xyz;
	float3 toEye = float3(0, 0, -1);

    float3 ratio = normalize(float3(gScreenSize.y, gScreenSize.x, 0));

	float3 dirP0P2 = normalize(cross(toEye, tangent) * ratio);
	dirP0P2 = dirP0P2 * ratio * gLineWidth;

	PosColorOut outVert;

	outVert.color = input[0].color;
	outVert.position = float4(p0.xyz - dirP0P2, 1) * input[0].position.w;
    output.Append(outVert);

	outVert.color = input[1].color;
	outVert.position = float4(p1.xyz - dirP0P2, 1) * input[1].position.w;
    output.Append(outVert);

	outVert.color = input[0].color;
	outVert.position = float4(p0.xyz + dirP0P2, 1) * input[0].position.w;
    output.Append(outVert);

	outVert.color = input[1].color;
	outVert.position = float4(p1.xyz + dirP0P2, 1) * input[1].position.w;
    output.Append(outVert);
}

#endif //LINE_WIDTH_GS_HLSL
