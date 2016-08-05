#pragma once

/// Initializes the mesh based on data passed in.
/// Vertices contain a position.
/// \param verts - The vertices to copy to the vertex buffer.
/// \param numVerts - The number of vertices to copy to the vertex buffer.
/// \param primitiveType - The type of primitive represented by the vertices.
template <typename VertexFormat>
void RenderMesh::CreateMesh(const VertexFormat *verts, unsigned int numVerts, 
							D3D_PRIMITIVE_TOPOLOGY _primitiveType)
{
	numVertices = numVerts;
	primitiveType = _primitiveType;
	numPrimitives = numVerts / 3;

	startVertex = VertexBufferManager::GetReference().GetVertexBuffer<VertexFormat>().AddVerts(verts, numVerts);

	EDMath::ComputeSphere( boundingSphere, &verts[0].position.x, numVerts, sizeof(VertexFormat) );

	EDMath::ComputeAabb( boundingAabb, &verts[0].position.x, numVerts, sizeof(VertexFormat) );
}

/// Initializes the indexed mesh based on data passed in.
/// Vertices that contain a position and texture coordinate.
/// \param verts - The vertices to copy to the vertex buffer.
/// \param numVerts - The number of vertices to copy to the vertex buffer.
/// \param indices - The indices to copy to the index buffer.
/// \param numIndices - The number of indices to copy to the index buffer.
/// \param primitiveType - The type of primitive represented by the vertices.
template <typename VertexFormat>
void RenderMesh::CreateIndexedMesh(const VertexFormat *verts, unsigned int numVerts, 
									const unsigned int *indices, unsigned int numIndices, 
									D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	CreateMesh<VertexFormat>(verts, numVerts, primitiveType);
	numPrimitives = numIndices / 3;

	startIndex = IndexBuffer::GetReference().AddIndices(indices, numIndices);
}
