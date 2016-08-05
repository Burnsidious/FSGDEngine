#include "precompiled.h"
#include "DebugRenderer.h"
#include "../EDUtilities/InternalOutput.h"
#include "../FSGDGame/RenderController.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/RenderShapeDebug.h"
#include "../EDRendererD3D/VertexFormats.h"
#include "../EDMath/float3.h"
#include "../EDMath/float4x4.h"
#include "../EDMath/capsule.h"
#include "../EDMath/obb.h"

using namespace EDRendererD3D;
using namespace EDMath;
using namespace EDUtilities;
using namespace DirectX;

namespace EDRendererD3D
{
	DebugRenderer* DebugRenderer::instancePtr = 0;


	DebugRenderer::DebugRenderer(void)
	{
		linesShapePtr = nullptr;

		std::vector< EDUtilities::ContentHandle<RenderForm> > forms;
		forms.push_back(EDUtilities::ContentHandle<RenderForm>());
		forms[0] = ContentManager::LoadXML<RenderForm>("GDForm/frm_DebugLines.xml");

		// Lines
		linesShapePtr = static_cast<RenderShapeDebug *>(forms[0].GetContent()->GetNewRenderShape());
		linesShapePtr->Initialize(forms);
		linesShapePtr->RenderFunc = RenderShapeDebug::RenderDebugLines;

		currentLineBufferCount = 0;
		// Triangles
		forms[0] = ContentManager::LoadXML<RenderForm>("GDForm/frm_DebugTriangles.xml");
		trianglesShapePtr = static_cast<RenderShapeDebug *>(forms[0].GetContent()->GetNewRenderShape());
		trianglesShapePtr->Initialize(forms);
		trianglesShapePtr->RenderFunc = RenderShapeDebug::RenderDebugTriangles;

		currentTriBufferCount = 0;

		// Create and initialize debug constant buffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cbDebug);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );

		debugBuffer.gLineWidth = 0.01f;
		InitData.pSysMem = &debugBuffer;

		HR(Renderer::theDevicePtr->CreateBuffer(&bd, &InitData, &debugCBuffer));
		Renderer::theContextPtr->GSSetConstantBuffers(
			debugBuffer.REGISTER_SLOT, 1, &debugCBuffer.p);
		
	}

	DebugRenderer::~DebugRenderer()
	{
		delete linesShapePtr;
		delete trianglesShapePtr;
	}

	DebugRenderer* DebugRenderer::GetInstance(void)
	{
		if( instancePtr == 0 )
			instancePtr = new DebugRenderer;

		return instancePtr;
	}

	void DebugRenderer::DeleteInstance(void)
	{
		delete instancePtr;
		instancePtr = 0;
	}

	void DebugRenderer::DrawAabb(const EDMath::Float3& vmin, const EDMath::Float3& vmax, const XMFLOAT4 &color)
	{
		EDMath::Aabb aabb;
		aabb.center = (vmin + vmax) * 0.5f;
		aabb.extents = vmax - aabb.center;
		DrawAabb( aabb, color );
	}

	void DebugRenderer::DrawAabb(const EDMath::Aabb& aabb, const XMFLOAT4 &color)
	{
		const int NUM_POINTS = 8;
		VERTEX_POSCOLOR points[NUM_POINTS];
		
		for(unsigned int i = 0; i < NUM_POINTS; ++i)
		{
			points[i].color = color;
		}

		points[0].position = XMFLOAT3(aabb.Min().v);
		points[7].position = XMFLOAT3(aabb.Max().v);

		points[1].position = XMFLOAT3( points[0].position.x, points[0].position.y, points[7].position.z );
		points[2].position = XMFLOAT3( points[0].position.x, points[7].position.y, points[0].position.z );
		points[3].position = XMFLOAT3( points[0].position.x, points[7].position.y, points[7].position.z );

		points[4].position = XMFLOAT3( points[7].position.x, points[0].position.y, points[0].position.z );
		points[5].position = XMFLOAT3( points[7].position.x, points[0].position.y, points[7].position.z );
		points[6].position = XMFLOAT3( points[7].position.x, points[7].position.y, points[0].position.z );

		DrawSegment( points[0], points[1]);	
		DrawSegment( points[1], points[3]);	
		DrawSegment( points[3], points[2]);	
		DrawSegment( points[2], points[0]);	

		DrawSegment( points[4], points[5]);	
		DrawSegment( points[5], points[7]);	
		DrawSegment( points[7], points[6]);	
		DrawSegment( points[6], points[4]);	

		DrawSegment( points[0], points[4]);	
		DrawSegment( points[1], points[5]);	
		DrawSegment( points[2], points[6]);	
		DrawSegment( points[3], points[7]);	
	}

	void DebugRenderer::DrawObb(const EDMath::Obb& obb, const DirectX::XMFLOAT4 &color)
	{
		Float3 points[8];

		points[0] = obb.extents * -1.0f;
		points[7] = obb.extents;

		points[1] = Float3( points[0].x, points[0].y, points[7].z );
		points[2] = Float3( points[0].x, points[7].y, points[0].z );
		points[3] = Float3( points[0].x, points[7].y, points[7].z );

		points[4] = Float3( points[7].x, points[0].y, points[0].z );
		points[5] = Float3( points[7].x, points[0].y, points[7].z );
		points[6] = Float3( points[7].x, points[7].y, points[0].z );
		
		for(unsigned int i = 0; i < 8; ++i)
			TransformPoint( points[i], points[i], obb.transform );

		DrawSegment( points[0], points[1], color );	
		DrawSegment( points[1], points[3], color );	
		DrawSegment( points[3], points[2], color );	
		DrawSegment( points[2], points[0], color );	
						   
		DrawSegment( points[4], points[5], color );	
		DrawSegment( points[5], points[7], color );	
		DrawSegment( points[7], points[6], color );	
		DrawSegment( points[6], points[4], color );	
						   				
		DrawSegment( points[0], points[4], color );	
		DrawSegment( points[1], points[5], color );	
		DrawSegment( points[2], points[6], color );	
		DrawSegment( points[3], points[7], color );	
	}

	void DebugRenderer::DrawSphere(const EDMath::Sphere& sphere, const DirectX::XMFLOAT4 &color)
	{
		DrawSphere( sphere.center, sphere.radius, color );
	}

	void DebugRenderer::DrawSphere(const EDMath::Float3& center, float radius, const DirectX::XMFLOAT4 &color)
	{
		static const unsigned int PT_COUNT = 15;
		static const Float3 points[][PT_COUNT] =
		{
			{
				Float3(		1.0f,		0.0f,	0.0f		),
				Float3(		0.9238f,	0.0f,	0.3827f		),
				Float3(		0.7071f,	0.0f,	0.7071f		),
				Float3(		0.3827f,	0.0f,	0.98238f	),
				Float3(		0.0f,		0.0f,	1.0f		),
				Float3(		-0.3827f,	0.0f,	0.98238f	),
				Float3(		-0.7071f,	0.0f,	0.7071f		),
				Float3(		-0.9238f,	0.0f,	0.3827f		),
				Float3(		-1.0f,		0.0f,	0.0f		),
				Float3(		-0.9238f,	0.0f,	-0.3827f	),
				Float3(		-0.7071f,	0.0f,	-0.7071f	),
				Float3(		-0.3827f,	0.0f,	-0.98238f	),
				Float3(		0.0f,		0.0f,	-1.0f		),
				Float3(		0.3827f,	0.0f,	-0.98238f	),
				Float3(		0.7071f,	0.0f,	-0.7071f	)
			},	
			{	
				Float3(		1.0f,		0.0f,		0.0f	),
				Float3(		0.9238f,	0.3827f,	0.0f	),
				Float3(		0.7071f,	0.7071f,	0.0f	),
				Float3(		0.3827f,	0.98238f,	0.0f	),
				Float3(		0.0f,		1.0f,		0.0f	),
				Float3(		-0.3827f,	0.98238f,	0.0f	),
				Float3(		-0.7071f,	0.7071f,	0.0f	),
				Float3(		-0.9238f,	0.3827f,	0.0f	),
				Float3(		-1.0f,		0.0f,		0.0f	),
				Float3(		-0.9238f,	-0.3827f,	0.0f	),
				Float3(		-0.7071f,	-0.7071f,	0.0f	),
				Float3(		-0.3827f,	-0.98238f,	0.0f	),
				Float3(		0.0f,		-1.0f,		0.0f	),
				Float3(		0.3827f,	-0.98238f,	0.0f	),
				Float3(		0.7071f,	-0.7071f,	0.0f	)
			},	
			{	
				Float3(		0.0f,	1.0f,		0.0f		),
				Float3(		0.0f,	0.9238f,	0.3827f		),
				Float3(		0.0f,	0.7071f,	0.7071f		),
				Float3(		0.0f,	0.3827f,	0.98238f	),
				Float3(		0.0f,	0.0f,		1.0f		),
				Float3(		0.0f,	-0.3827f,	0.98238f	),
				Float3(		0.0f,	-0.7071f,	0.7071f		),
				Float3(		0.0f,	-0.9238f,	0.3827f		),
				Float3(		0.0f,	-1.0f,		0.0f		),
				Float3(		0.0f,	-0.9238f,	-0.3827f	),
				Float3(		0.0f,	-0.7071f,	-0.7071f	),
				Float3(		0.0f,	-0.3827f,	-0.98238f	),
				Float3(		0.0f,	0.0f,		-1.0f		),
				Float3(		0.0f,	0.3827f,	-0.98238f	),
				Float3(		0.0f,	0.7071f,	-0.7071f	)
			},
		};

		VERTEX_POSCOLOR a;
		VERTEX_POSCOLOR b;

		a.color = color;
		b.color = color;

		for(unsigned int i = 0; i < 3; ++i)
		{
			Float3 start = points[i][PT_COUNT-1];
			for(unsigned int j = 0; j < PT_COUNT; ++j)
			{
				a.position = XMFLOAT3( (center + start * radius).v );
				b.position = XMFLOAT3( (center + points[i][j] * radius).v );
				start = points[i][j];

				DrawSegment( a, b );
			}
		}
	}

	void DebugRenderer::DrawCapsule(const EDMath::Capsule& capsule, const DirectX::XMFLOAT4 &color)
	{
		static const unsigned int PT_COUNT = 15;
		static const Float3 points[][PT_COUNT] =
		{
			{
				Float3(		1.0f,		0.0f,	0.0f		),
				Float3(		0.9238f,	0.0f,	0.3827f		),
				Float3(		0.7071f,	0.0f,	0.7071f		),
				Float3(		0.3827f,	0.0f,	0.98238f	),
				Float3(		0.0f,		0.0f,	1.0f		),
				Float3(		-0.3827f,	0.0f,	0.98238f	),
				Float3(		-0.7071f,	0.0f,	0.7071f		),
				Float3(		-0.9238f,	0.0f,	0.3827f		),
				Float3(		-1.0f,		0.0f,	0.0f		),
				Float3(		-0.9238f,	0.0f,	-0.3827f	),
				Float3(		-0.7071f,	0.0f,	-0.7071f	),
				Float3(		-0.3827f,	0.0f,	-0.98238f	),
				Float3(		0.0f,		0.0f,	-1.0f		),
				Float3(		0.3827f,	0.0f,	-0.98238f	),
				Float3(		0.7071f,	0.0f,	-0.7071f	)
			},	
			{	
				Float3(		1.0f,		0.0f,		0.0f	),
				Float3(		0.9238f,	0.3827f,	0.0f	),
				Float3(		0.7071f,	0.7071f,	0.0f	),
				Float3(		0.3827f,	0.98238f,	0.0f	),
				Float3(		0.0f,		1.0f,		0.0f	),
				Float3(		-0.3827f,	0.98238f,	0.0f	),
				Float3(		-0.7071f,	0.7071f,	0.0f	),
				Float3(		-0.9238f,	0.3827f,	0.0f	),
				Float3(		-1.0f,		0.0f,		0.0f	),
				Float3(		-0.9238f,	-0.3827f,	0.0f	),
				Float3(		-0.7071f,	-0.7071f,	0.0f	),
				Float3(		-0.3827f,	-0.98238f,	0.0f	),
				Float3(		0.0f,		-1.0f,		0.0f	),
				Float3(		0.3827f,	-0.98238f,	0.0f	),
				Float3(		0.7071f,	-0.7071f,	0.0f	)
			},	
			{	
				Float3(		0.0f,	1.0f,		0.0f		),
				Float3(		0.0f,	0.9238f,	0.3827f		),
				Float3(		0.0f,	0.7071f,	0.7071f		),
				Float3(		0.0f,	0.3827f,	0.98238f	),
				Float3(		0.0f,	0.0f,		1.0f		),
				Float3(		0.0f,	-0.3827f,	0.98238f	),
				Float3(		0.0f,	-0.7071f,	0.7071f		),
				Float3(		0.0f,	-0.9238f,	0.3827f		),
				Float3(		0.0f,	-1.0f,		0.0f		),
				Float3(		0.0f,	-0.9238f,	-0.3827f	),
				Float3(		0.0f,	-0.7071f,	-0.7071f	),
				Float3(		0.0f,	-0.3827f,	-0.98238f	),
				Float3(		0.0f,	0.0f,		-1.0f		),
				Float3(		0.0f,	0.3827f,	-0.98238f	),
				Float3(		0.0f,	0.7071f,	-0.7071f	)
			},
		};

		Float4x4 xform;
		xform.makeIdentity();

		Float3& xAxis = xform.XAxis;
		Float3& yAxis = xform.YAxis;
		Float3& zAxis = xform.ZAxis;

		yAxis = capsule.direction;

		if( abs(capsule.direction.y) > 0.9f )
		{	
			CrossProduct( zAxis, Float3(1.0f, 0.0f, 0.0f), yAxis );
			CrossProduct( xAxis, yAxis, zAxis );
			zAxis.normalize();
			xAxis.normalize();
		}
		else
		{
			CrossProduct( zAxis, yAxis, Float3(0.0f, 1.0f, 0.0f) );
			CrossProduct( xAxis, yAxis, zAxis );
			zAxis.normalize();
			xAxis.normalize();
		}

		VERTEX_POSCOLOR a;
		VERTEX_POSCOLOR b;

		a.color = color;
		b.color = color;

		Float3 yOffset = yAxis * capsule.height * 0.5f;
		Float3 xOffset = xAxis * capsule.radius;
		Float3 zOffset = zAxis * capsule.radius;

		a.position = XMFLOAT3( (capsule.center + yOffset + xOffset).v );
		b.position = XMFLOAT3( (capsule.center - yOffset + xOffset).v );
		DrawSegment( a, b );

		a.position = XMFLOAT3( (capsule.center + yOffset - xOffset).v );
		b.position = XMFLOAT3( (capsule.center - yOffset - xOffset).v );
		DrawSegment( a, b );

		a.position = XMFLOAT3( (capsule.center + yOffset + zOffset).v );
		b.position = XMFLOAT3( (capsule.center - yOffset + zOffset).v );
		DrawSegment( a, b );
		
		a.position = XMFLOAT3( (capsule.center + yOffset - zOffset).v );
		b.position = XMFLOAT3( (capsule.center - yOffset - zOffset).v );
		DrawSegment( a, b );

		Float3 start = points[0][14];
		Float3 startPos;
		Float3 endPos;
		for(unsigned int i = 0; i < 15; ++i)
		{
			TransformPoint( startPos, start, xform );
			TransformPoint( endPos, points[0][i], xform );
			start = points[0][i];
			
			a.position = XMFLOAT3( (capsule.center + yOffset + startPos * capsule.radius).v );
			b.position = XMFLOAT3( (capsule.center + yOffset + endPos * capsule.radius).v );

			DrawSegment( a, b );

			a.position = XMFLOAT3( (capsule.center - yOffset + startPos * capsule.radius).v );
			b.position = XMFLOAT3( (capsule.center - yOffset + endPos * capsule.radius).v );

			DrawSegment( a, b );
		}


		for(unsigned int j = 0; j < 8; ++j)
		{
			TransformPoint( startPos, points[1][j], xform );
			TransformPoint( endPos, points[1][j+1], xform );
			
			a.position = XMFLOAT3( (capsule.center + yOffset + startPos * capsule.radius).v );
			b.position = XMFLOAT3( (capsule.center + yOffset + endPos * capsule.radius).v );

			DrawSegment( a, b );

			TransformPoint( startPos, points[1][j] * -1.0f, xform );
			TransformPoint( endPos, points[1][j+1] * -1.0f, xform );
			
			a.position = XMFLOAT3( (capsule.center - yOffset + startPos * capsule.radius).v );
			b.position = XMFLOAT3( (capsule.center - yOffset + endPos * capsule.radius).v );

			DrawSegment( a, b );
		}

		for(unsigned int j = 4; j < 12; ++j)
		{
			TransformPoint( startPos, points[2][j] * -1.0f, xform );
			TransformPoint( endPos, points[2][j+1] * -1.0f, xform );
			
			a.position = XMFLOAT3( (capsule.center + yOffset + startPos * capsule.radius).v );
			b.position = XMFLOAT3( (capsule.center + yOffset + endPos * capsule.radius).v );

			DrawSegment( a, b );

			TransformPoint( startPos, points[2][j], xform );
			TransformPoint( endPos, points[2][j+1], xform );
			
			a.position = XMFLOAT3( (capsule.center - yOffset + startPos * capsule.radius).v );
			b.position = XMFLOAT3( (capsule.center - yOffset + endPos * capsule.radius).v );

			DrawSegment( a, b );
		}
	}

	//void DebugRenderer::DrawTriangle(const EDCollision::Triangle& triangle, const XMFLOAT4 &color)
	//{
	//	Float3 centroid(0.0f, 0.0f, 0.0f);
	//	VERTEX_POSCOLOR one;
	//	one.color = color;
	//	VERTEX_POSCOLOR two;
	//	two.color = color;

	//	for(unsigned int i = 0; i < 3; ++i)
	//	{
	//		one.position = XMFLOAT3(triangle.GetVertex(i).v);
	//		two.position = XMFLOAT3(triangle.GetVertex((1<<i) & 0x3).v);
	//		DrawSegment( one, two );
	//		centroid += triangle.GetVertex(i);
	//	}
	//
	//	centroid *= (1.0f/3.0f);
	//	one.position = XMFLOAT3(centroid.v);
	//	two.position = XMFLOAT3((centroid + triangle.GetNormal()).v);
	//	DrawSegment( one, two );
	//}

	void DebugRenderer::DrawSegment(const EDMath::Float3& p1, const EDMath::Float3& p2, const DirectX::XMFLOAT4 &color)
	{
		VERTEX_POSCOLOR points[2];

		points[0].color = points[1].color = color;

		points[0].position = XMFLOAT3( p1.v );
		points[1].position = XMFLOAT3( p2.v );

		DrawLines(points, 1);
	}

	void DebugRenderer::DrawSegment(const VERTEX_POSCOLOR& p1, 
		const EDRendererD3D::VERTEX_POSCOLOR& p2)
	{
		VERTEX_POSCOLOR points[2];
		points[0] = p1;
		points[1] = p2;
		DrawLines(points, 1);
	}

	void DebugRenderer::DrawLines(const VERTEX_POSCOLOR *_points, UINT numLines)
	{
		for(UINT i = 0; i < numLines*2; ++i)
		{
			linePoints.push_back(_points[i]);
		}
	}

	void DebugRenderer::DrawMatrix(const EDMath::Float4x4 &mat, float scale)
	{
		VERTEX_POSCOLOR p1;
		VERTEX_POSCOLOR p2;

		p1.position = *(XMFLOAT3 *)&mat.WAxis;

		p2.position = *(XMFLOAT3 *)& (mat.WAxis + mat.ZAxis * scale);
		p1.color = p2.color = XMFLOAT4(0, 0, 1, 1);
		p2.color = XMFLOAT4(0, 0, 1, 0);
		DrawSegment(p1, p2);

		p2.position = *(XMFLOAT3 *)& (mat.WAxis + mat.YAxis * scale);
		p1.color = p2.color = XMFLOAT4(0, 1, 0, 1);
		p2.color = XMFLOAT4(0, 1, 0, 0);
		DrawSegment(p1, p2);

		p2.position = *(XMFLOAT3 *)& (mat.WAxis + mat.XAxis * scale);
		p1.color = p2.color = XMFLOAT4(1, 0, 0, 1);
		p2.color = XMFLOAT4(1, 0, 0, 0);
		DrawSegment(p1, p2);
	}

	void DebugRenderer::DrawTriangles(const EDRendererD3D::VERTEX_POSCOLOR *_points, UINT numTriangles)
	{
		for(UINT i = 0; i < numTriangles*3; ++i)
		{
			trianglePoints.push_back(_points[i]);
		}
	}

	//void DebugRenderer::DrawFrustum(const EDCollision::FrustumHull& frustum,
	//	const DirectX::XMFLOAT4 &color)
	//{
	//	VERTEX_POSCOLOR one;
	//	one.color = color;
	//	VERTEX_POSCOLOR two;
	//	two.color = color;

	//	one.position = XMFLOAT3(frustum.points[0].v);
	//	two.position = XMFLOAT3(frustum.points[1].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[1].v);
	//	two.position = XMFLOAT3(frustum.points[3].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[3].v);
	//	two.position = XMFLOAT3(frustum.points[2].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[2].v);
	//	two.position = XMFLOAT3(frustum.points[0].v);
	//	DrawSegment( one, two );
	//
	//	one.position = XMFLOAT3(frustum.points[4].v);
	//	two.position = XMFLOAT3(frustum.points[5].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[5].v);
	//	two.position = XMFLOAT3(frustum.points[7].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[7].v);
	//	two.position = XMFLOAT3(frustum.points[6].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[6].v);
	//	two.position = XMFLOAT3(frustum.points[4].v);
	//	DrawSegment( one, two );
	//
	//	one.position = XMFLOAT3(frustum.points[0].v);
	//	two.position = XMFLOAT3(frustum.points[4].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[1].v);
	//	two.position = XMFLOAT3(frustum.points[5].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[2].v);
	//	two.position = XMFLOAT3(frustum.points[6].v);
	//	DrawSegment( one, two );

	//	one.position = XMFLOAT3(frustum.points[3].v);
	//	two.position = XMFLOAT3(frustum.points[7].v);
	//	DrawSegment( one, two );
	//}

	void DebugRenderer::RenderDebugPrimitives(void)
	{
		// Lines
		unsigned int pointCount = static_cast<unsigned int>(linePoints.size());
		if(pointCount)
		{
			if(currentLineBufferCount < pointCount)
			{
				currentLineBufferCount = pointCount;

				lineVertexBufferPtr.Release();
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = currentLineBufferCount * sizeof(VERTEX_POSCOLOR);
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				Renderer::theDevicePtr->CreateBuffer(&desc, 0, &lineVertexBufferPtr);
			}
			D3D11_MAPPED_SUBRESOURCE edit;
			Renderer::theContextPtr->Map(lineVertexBufferPtr, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
			memcpy(edit.pData, &linePoints[0], pointCount * sizeof(VERTEX_POSCOLOR));
			Renderer::theContextPtr->Unmap(lineVertexBufferPtr, 0);
			linePoints.clear();

			linesShapePtr->SetLinesPointCount(pointCount);
			linesShapePtr->SetLinesVertexBuffer(lineVertexBufferPtr);
			linesShapePtr->AddToContextSet();
		}

		// Triangles
		pointCount = static_cast<unsigned int>(trianglePoints.size());
		if(pointCount)
		{
			if(currentTriBufferCount < pointCount)
			{
				currentTriBufferCount = pointCount;
				D3D11_BUFFER_DESC desc;
				desc.ByteWidth = currentTriBufferCount * sizeof(VERTEX_POSCOLOR);
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				Renderer::theDevicePtr->CreateBuffer(&desc, 0, &triangleVertexBufferPtr);
			}

			D3D11_MAPPED_SUBRESOURCE edit;
			Renderer::theContextPtr->Map(triangleVertexBufferPtr, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
			memcpy(edit.pData, &trianglePoints[0], pointCount * sizeof(VERTEX_POSCOLOR));
			Renderer::theContextPtr->Unmap(triangleVertexBufferPtr, 0);
			trianglePoints.clear();

			trianglesShapePtr->SetTrianglesPointCount(pointCount);
			trianglesShapePtr->SetTrianglesVertexBuffer(triangleVertexBufferPtr);
			trianglesShapePtr->AddToContextSet();
		}
	}

	void DebugRenderer::SetLineWidth(float width)
	{
		debugBuffer.gLineWidth = width;

		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(debugCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &debugBuffer, sizeof(debugBuffer));
		Renderer::theContextPtr->Unmap(debugCBuffer, 0);

		Renderer::theContextPtr->GSSetConstantBuffers(
			debugBuffer.REGISTER_SLOT, 1, &debugCBuffer.p);
	}
}