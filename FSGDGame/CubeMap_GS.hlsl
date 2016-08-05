#ifndef CUBE_MAP_GS_HLSL
#define CUBE_MAP_GS_HLSL

#include "VertexLayouts.hlsli"
#include "ShadowSupport.hlsli"
#include "../EDRendererD3D/LightBuffers.h"
#include "../EDRendererD3D/ConstantBuffers.h"

[maxvertexcount(18)]
void main(triangle PointDepthVertOut input[3],
		  inout TriangleStream<DepthGeoOut> CubeMapStream )
{
	float4 inPos;

    for( int f = 0; f < 6; ++f )
    {
        // Compute screen coordinates
        DepthGeoOut output;
        output.RTIndex = f;
        for( int v = 0; v < 3; v++ )
        {
			inPos = input[v].position;
			inPos.xyz -= input[v].normalW * ShadowNormOffset;
			output.position = mul(inPos, pointLightShadowData.viewProjection[f]);

			output.depth.xy = output.position.zw;
			output.texCoord = input[v].texCoord;

            CubeMapStream.Append( output );
        }
        CubeMapStream.RestartStrip();
    }
}

#endif //CUBE_MAP_GS_HLSL