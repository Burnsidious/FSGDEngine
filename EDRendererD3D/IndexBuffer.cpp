#include "precompiled.h"
#include "IndexBuffer.h"
#include "Renderer.h"


namespace EDRendererD3D
{
    IndexBuffer *IndexBuffer::instancePtr = 0;

    IndexBuffer::IndexBuffer(void)
    {
        indexBufferPtr = 0;
    }

    IndexBuffer::~IndexBuffer(void)
    {
		ReleaseCOM(indexBufferPtr);
    }

    IndexBuffer &IndexBuffer::GetReference()
    {
        if(!instancePtr)
        {
			instancePtr = new IndexBuffer;
        }
        return *instancePtr;
    }

    void IndexBuffer::DeleteInstance()
    {
		delete instancePtr;
    }

    UINT IndexBuffer::AddIndices(const UINT *_indices, UINT _numIndices)
    {
		UINT ret = 0;
	
		ret = AddIndicesSolution(_indices, _numIndices);

		return ret;
    }
}