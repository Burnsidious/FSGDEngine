#include "precompiled.h"

#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDUtilities/PrintConsole.h"
//#include "../EDMath/Float3.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;

#include <assert.h>

namespace EDRendererD3D
{
	UINT IndexBuffer::AddIndicesSolution(const UINT *_indices, UINT _numIndices)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"IndexBuffer::AddIndicesSolution");
			added = true;
		}
		UINT ret = 0;

		/// Test if we have a buffer yet
		if(indexBufferPtr)
		{
			D3D11_BUFFER_DESC ibd;

			indexBufferPtr->GetDesc(&ibd);

			D3D11_SUBRESOURCE_DATA iinitData;
			UINT oldBuffeSize = ibd.ByteWidth / sizeof(UINT);
			UINT newBuffeSize = oldBuffeSize + _numIndices;
			ret = oldBuffeSize;
			iinitData.pSysMem = new UINT[newBuffeSize];
			//memcpy((char *)(iinitData.pSysMem) + 100, 
			memcpy((char *)(iinitData.pSysMem) + ibd.ByteWidth, 
				_indices, sizeof(UINT) * _numIndices);

			ibd.ByteWidth += sizeof(UINT) * _numIndices;

			ID3D11Buffer *newIndexBufferPtr;

			HR(Renderer::theDevicePtr->CreateBuffer(&ibd, &iinitData, &newIndexBufferPtr));

			SetD3DName(indexBufferPtr, "The Index Buffer");

			Renderer::theContextPtr->CopySubresourceRegion(newIndexBufferPtr, 0, 0, 0, 0,
				indexBufferPtr, 0, 0);

			ReleaseCOM(indexBufferPtr);
			indexBufferPtr = newIndexBufferPtr;

			delete [] iinitData.pSysMem;
		}
		/// Have to create the initial buffer in this case
		else
		{
			D3D11_BUFFER_DESC ibd;
			ibd.Usage = D3D11_USAGE_DEFAULT;
			ibd.ByteWidth = sizeof(UINT) * _numIndices;
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			ibd.MiscFlags = 0;
			ibd.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA iinitData;
			iinitData.pSysMem = _indices;
			HR(Renderer::theDevicePtr->CreateBuffer(&ibd, &iinitData, &indexBufferPtr));
		}
		return ret;
	}
}