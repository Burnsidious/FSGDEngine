#pragma once



#include "VertexFormats.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDMath/sphere.h"
#include "../EDMath/aabb.h"
#include "../EDMath/aabb.h"

namespace EDRendererD3D
{
	/// A static mesh implementation.
    /// RenderMesh instances use the VertexBuffer and IndexBuffer classes to store their buffers.
    class RenderMesh
    {
    private:
		/// The number of primitives defined in this mesh
        UINT                        numPrimitives;
		/// The type of primitive contained in this mesh such as D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        D3D_PRIMITIVE_TOPOLOGY      primitiveType;
		/// The index location of the first vertex in the vertex buffer for this mesh.
        UINT                        startVertex;
		/// The index location of the first index in the index buffer for this mesh.
        UINT                        startIndex;
		/// The number of vertices in this mesh.
        UINT                        numVertices;
		/// A bounding volume surrounding this mesh.  
        /// This is local to the mesh and should not be transformed.
		EDMath::Sphere				boundingSphere;

		EDMath::Aabb				boundingAabb;

		static bool LoadXML(RenderMesh* out, const char* xmlFileNamePtr, const char* pVertexFormat);

		template<typename T>
		static bool LoadContent(RenderMesh* resultPtr, const char* xmlFileNamePtr, 
			EDUtilities::ContentHandle<T> &meshHandle, const char* pVertexFormat);
    public:

		/// Initializes members to default states.
        RenderMesh();
		/// Releases D3D API objects.
        ~RenderMesh();

		RenderMesh &operator=(const RenderMesh &rhs);
		RenderMesh &operator=(RenderMesh &&rhs);


        /// Initializes the mesh based on data passed in.
		/// Vertices contain a position.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
		template <typename VertexFormat>
		void CreateMesh(const VertexFormat *verts, unsigned int numVerts, 
			D3D_PRIMITIVE_TOPOLOGY _primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        /// Initializes the indexed mesh based on data passed in.
		/// Vertices that contain a position and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param indices - The indices to copy to the index buffer.
		/// \param numIndices - The number of indices to copy to the index buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
		template <typename VertexFormat>
        void CreateIndexedMesh(const VertexFormat *verts, unsigned int numVerts, 
			const unsigned int *indices, unsigned int numIndices, 
			D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        inline unsigned int GetPrimitiveCount() const { return numPrimitives; }
        inline D3D_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return primitiveType; }
        inline unsigned int GetStartVertex() const { return startVertex; }
		
        inline unsigned int GetStartIndex() const { return startIndex; }
        inline unsigned int GetVertCount() const { return numVertices; }
        inline const EDMath::Sphere &GetSphere(void) const { return boundingSphere; }
		inline const EDMath::Aabb &GetAabb(void) const { return boundingAabb; }

		/// Creates a new indexed mesh in heap memory based on a file.
		/// \param fileName - The name of the file to read the mesh from.
		/// \param pVertexFormat - A string denoting the vertex format to be used, must exist
        /// in the array defined in VertexFormats.cpp.
		/// \return Returns the mesh.  This mesh will need to be deleted.
        static RenderMesh *Load(const char* fileName, const char* pVertexFormat);

		/// Loads a RenderMesh from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderMesh to load from XML format.
		/// \param pVertexFormat - A string denoting the vertex format to be used, must exist
        /// in the array defined in VertexFormats.cpp.
		static RenderMesh *LoadXML(const char* xmlFileNamePtr, const char* pVertexFormat);

		static RenderMesh LoadXMLObject(const char* xmlFileNamePtr, const char* pVertexFormat);

    };

#include "RenderMesh.hpp"
}