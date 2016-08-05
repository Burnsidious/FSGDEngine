#pragma once
#include "RenderShape.h"



namespace EDRendererD3D
{
	class RenderShapeDebug : public RenderShape
	{
	public:
		RenderShapeDebug(void);
		~RenderShapeDebug(void);

		static void RenderDebugLines(RenderNode &node);
		static void RenderDebugTriangles(RenderNode &node);

		void SetLinesVertexBuffer(ID3D11Buffer *buffer) { linesVertexBufferPtr = buffer; }
		void SetLinesPointCount(unsigned int _pointCount) { linesPointCount = _pointCount; }

		void SetTrianglesVertexBuffer(ID3D11Buffer *buffer) { trianglesVertexBufferPtr = buffer; }
		void SetTrianglesPointCount(unsigned int _pointCount) { trianglesPointCount = _pointCount; }

	private:
		ID3D11Buffer *linesVertexBufferPtr;
		unsigned int linesPointCount;

		ID3D11Buffer *trianglesVertexBufferPtr;
		unsigned int trianglesPointCount;
	};
}