#pragma once



namespace EDRendererD3D
{
    /// A singleton class used to hold the index buffers used for rendering indexed geometry.
    class IndexBuffer
    {
    private:
        IndexBuffer(void);
        IndexBuffer(const IndexBuffer &) {}
        IndexBuffer &operator=(const IndexBuffer &) { return *this; }

        /// the one and only instance.
        static IndexBuffer *instancePtr;

        /// The buffer used for all indices.
		ID3D11Buffer *indexBufferPtr;

    public:
        ~IndexBuffer(void);

        /// Gets a reference to the one and only IndexBuffer instance.
        static IndexBuffer &GetReference();

        /// Deletes the instance of the IndexBuffer.
        static void DeleteInstance();

        /// Adds new indices to the indexBuffer.
        /// \param indices - The indices to be added.
        /// \param numIndices - The number of indices to be added.
        /// \return Returns the location in the index buffer where the added 
        /// indices begin.  This is needed for rendering.
        UINT AddIndices(const UINT *_indices, UINT numIndices);

        inline ID3D11Buffer * const GetIndices() { return indexBufferPtr; }

        /// Solution declarations, ignore these
        UINT AddIndicesSolution(const UINT *_indices, UINT numIndices);

    };
}