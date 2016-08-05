#pragma once

#include "VertexBuffer.h"

namespace EDRendererD3D
{
    /// A singleton class used to manage Vertexbuffer instances.
	class VertexBufferManager
	{
	private:
        /// Creates the VertexBuffer instances currently being managed.
		VertexBufferManager(void);
		VertexBufferManager(const VertexBufferManager &){}
		VertexBufferManager &operator=(const VertexBufferManager &){}

        /// The one and only instance.
		static VertexBufferManager *instancePtr;

        /// The VertexBuffer used to store VERTEX_POS type vertices
		VertexBuffer<VERTEX_POS> positionBuffer;
        /// The VertexBuffer used to store VERTEX_POSCOLOR type vertices
		VertexBuffer<VERTEX_POSCOLOR> positionColorBuffer;
        /// The VertexBuffer used to store VERTEX_POSTEX type vertices
		VertexBuffer<VERTEX_POSTEX> positionTexBuffer;
        /// The VertexBuffer used to store VERTEX_POSNORMTEX type vertices
		VertexBuffer<VERTEX_POSNORMTEX> posNormTexBuffer;
        /// The VertexBuffer used to store VERTEX_POSNORMTANTEX type vertices
		VertexBuffer<VERTEX_POSNORMTANTEX> posNormTanTexBuffer;
        /// The VertexBuffer used to store VERTEX_POSBONEWEIGHT type vertices
		VertexBuffer<VERTEX_POSBONEWEIGHT> posBoneWeightBuffer;
		/// The VertexBuffer used to store VERTEX_POSBONEWEIGHTNORMTEX type vertices
		VertexBuffer<VERTEX_POSBONEWEIGHTNORMTEX> posNormBoneWeightTexBuffer;
		/// The VertexBuffer used to store VERTEX_POSBONEWEIGHTNORMTANTEX type vertices
		VertexBuffer<VERTEX_POSBONEWEIGHTNORMTANTEX> posNormBoneWeightTanTexBuffer;

	public:
		~VertexBufferManager(void);

        /// Gets a reference to the VertexBufferManager instance.
		static VertexBufferManager &GetReference();
        /// Deletes the VertexBufferManager instance.
		static void DeleteInstance();

		inline VertexBuffer<VERTEX_POS> &GetPositionBuffer() { return  positionBuffer; }
		inline VertexBuffer<VERTEX_POSCOLOR> &GetPositionColorBuffer() { return positionColorBuffer; }
		inline VertexBuffer<VERTEX_POSTEX> &GetPositionTexBuffer() { return positionTexBuffer; }
		inline VertexBuffer<VERTEX_POSNORMTEX> &GetPosNormTexBuffer() { return posNormTexBuffer; }
		inline VertexBuffer<VERTEX_POSNORMTANTEX> &GetPosNormTanTexBuffer() { return posNormTanTexBuffer; }
		inline VertexBuffer<VERTEX_POSBONEWEIGHT> &GetPosBoneWeightBuffer() { return posBoneWeightBuffer; }
		inline VertexBuffer<VERTEX_POSBONEWEIGHTNORMTEX> &GetPosBoneWeightNormTexBuffer() 
		{ return posNormBoneWeightTexBuffer; }
		inline VertexBuffer<VERTEX_POSBONEWEIGHTNORMTANTEX> &GetPosBoneWeightNormTanTexBuffer() 
		{ return posNormBoneWeightTanTexBuffer; }

		template <typename VertexFormat>
		VertexBuffer<VertexFormat> &GetVertexBuffer();
	};
}