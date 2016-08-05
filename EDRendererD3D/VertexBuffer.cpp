#include "VertexBuffer.h"

namespace EDRendererD3D
{
	template<>
	void VertexBuffer<VERTEX_POSCOLOR>::CreateVertexDeclaration()
	{
        D3DVERTEXELEMENT9 decl[] =
        {
            {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
            {0,12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
            D3DDECL_END()
        };
        Renderer::theDevicePtr->CreateVertexDeclaration(decl, &vertexDeclarationPtr);
	}

	template<>
	void VertexBuffer<VERTEX_POSTEX>::CreateVertexDeclaration()
	{
        D3DVERTEXELEMENT9 decl[] =
        {
            {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
            {0,12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
            D3DDECL_END()
        };
        Renderer::theDevicePtr->CreateVertexDeclaration(decl, &vertexDeclarationPtr);
	}

	template<>
	void VertexBuffer<VERTEX_POS>::CreateVertexDeclaration()
	{
        D3DVERTEXELEMENT9 decl[] =
        {
            {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
            D3DDECL_END()
        };
        Renderer::theDevicePtr->CreateVertexDeclaration(decl, &vertexDeclarationPtr);
	}

	template<>
	void VertexBuffer<VERTEX_POSNORMTEX>::CreateVertexDeclaration()
	{
        D3DVERTEXELEMENT9 decl[] =
        {
            {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
            {0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},
            {0,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
            D3DDECL_END()
        };
        Renderer::theDevicePtr->CreateVertexDeclaration(decl, &vertexDeclarationPtr);
	}

    template<>
	void VertexBuffer<VERTEX_POSNORMTANTEX>::CreateVertexDeclaration()
	{
        D3DVERTEXELEMENT9 decl[] =
        {
            {0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
            {0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},
            {0,24,D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TANGENT,0},
            {0,40,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
            D3DDECL_END()
        };
        Renderer::theDevicePtr->CreateVertexDeclaration(decl, &vertexDeclarationPtr);
	}
}