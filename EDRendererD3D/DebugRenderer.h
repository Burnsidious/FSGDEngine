#pragma once



#include "VertexFormats.h"
#include "ConstantBuffers.h"

namespace EDRendererD3D
{
	class RenderShapeDebug;
};

namespace EDMath
{
	class Float3;
	class Float4x4;
	struct Sphere;
	struct Aabb;
	struct Capsule;
	struct Obb;
}

namespace EDRendererD3D
{
	class DebugRenderer
	{	
	private:

		EDRendererD3D::RenderShapeDebug *linesShapePtr;
		CComPtr<ID3D11Buffer> lineVertexBufferPtr;
		std::vector<VERTEX_POSCOLOR> linePoints;
		unsigned int currentLineBufferCount;

		EDRendererD3D::RenderShapeDebug *trianglesShapePtr;
		CComPtr<ID3D11Buffer> triangleVertexBufferPtr;
		std::vector<VERTEX_POSCOLOR> trianglePoints;
		unsigned int currentTriBufferCount;

		DebugRenderer(void);
		DebugRenderer(const DebugRenderer&){}
		DebugRenderer& operator=(const DebugRenderer&){ return *this; }

		static DebugRenderer* instancePtr;
		CComPtr<ID3D11Buffer> debugCBuffer;
		cbDebug debugBuffer;

	public:
	
		~DebugRenderer(void);

		void DrawAabb(const EDMath::Aabb& aabb, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));
		void DrawAabb(const EDMath::Float3& vmin, const EDMath::Float3& vmax, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));
		void DrawObb(const EDMath::Obb& obb, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));

		void DrawSphere(const EDMath::Sphere& sphere, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));
		void DrawSphere(const EDMath::Float3& center, float radius, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));

		void DrawCapsule(const EDMath::Capsule& capsule, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));
		/// Draws a single line with vertex colors
		/// \param p1 - The first point of the line
		/// \param p2 - The second point of the line
		void DrawSegment(const EDRendererD3D::VERTEX_POSCOLOR& p1, 
			const EDRendererD3D::VERTEX_POSCOLOR& p2);

		void DrawSegment(const EDMath::Float3& p1, const EDMath::Float3& p2, const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));

		/// Draws a list of lines with vertex colors.
		/// This is a helper method to group similar functionality of 
		/// drawing debug lines.
		/// \param points - A list of points to draw lines between
		/// \param primCount - The number of lines to render
		/// \param color - The color to draw the lines
		void DrawLines(const EDRendererD3D::VERTEX_POSCOLOR *points, UINT numLines);


		void DrawTriangles(const EDRendererD3D::VERTEX_POSCOLOR *points, UINT numTriangles);

		//void DrawTriangle(const EDCollision::Triangle& triangle, 
		//	const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));
		//void DrawFrustum(const EDCollision::FrustumHull& frustum, 
		//	const DirectX::XMFLOAT4 &color = DirectX::XMFLOAT4(1, 1, 1, 1));

		void DrawMatrix(const EDMath::Float4x4 &mat, float scale = 1.0f);

		void RenderDebugPrimitives(void);

		/// Sets the width of all lines being rendered this frame.
		/// \param width - The width of the lines to be drawn this frame.  Units are in screen space
		/// where 2 would be the width of the screen, 1 is half the width and 1/screen width would be a single pixel wide
		void SetLineWidth(float width);

		static DebugRenderer* GetInstance(void);
		static void DeleteInstance(void);
	};
}