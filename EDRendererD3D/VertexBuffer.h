#pragma once

#include "VertexFormats.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	/// \param VertexFormat - One of the vertex structures declared in VertexFormats.h
	template <typename VertexFormat>
	/// A vertex buffer handling class.  Instances of this class can be used to share
	/// vertex buffers, reducing the number of times the vertex stream must be switched.
    /// Use the VertexBufferManager to access the instances of this class.
	class VertexBuffer
	{
        friend class VertexBufferManager;
	private:
		/// The buffer containing the vertices of this mesh.
		ID3D11Buffer *vertexBufferPtr;

		VertexBuffer(void);
        VertexBuffer(const VertexBuffer&) {}
        VertexBuffer &operator=(const VertexBuffer&) { return *this; }

	public:
		size_t testSize; 
		~VertexBuffer(void);

		/// Adds new vertices to the vertex buffer.
        /// \param verts - The vertices to be added.
        /// \param numVerts - The number of vertices to be added.
        /// \return Returns the location in the vertex buffer where the added 
        /// vertices begin.  This is needed for rendering.
        UINT AddVerts(const VertexFormat *verts, UINT _numVerts);

		inline ID3D11Buffer *const GetVertexBuffer() { return vertexBufferPtr; }
	};
}
#include "VertexBuffer.hpp"