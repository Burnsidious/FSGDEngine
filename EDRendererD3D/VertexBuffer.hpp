namespace EDRendererD3D
{
	template <typename VertexFormat>
	VertexBuffer<VertexFormat>::VertexBuffer(void) 
	{
		vertexBufferPtr = 0;
	}

	template <typename VertexFormat>
	VertexBuffer<VertexFormat>::~VertexBuffer(void) 
	{
		ReleaseCOM(vertexBufferPtr);
	}

	template <typename VertexFormat>
	UINT VertexBuffer<VertexFormat>::AddVerts(const VertexFormat *verts, UINT _numVerts)
	{
		size_t ret;
		// Test if this buffer has already been finalized
		if(!vertexBufferPtr)
		{
			ret = 0;
			//// Implement a solution for the Renderer Lab
			//for(size_t i = 0; i < _numVerts; ++i)
				//vertices.push_back(verts[i]);

			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DEFAULT;
			vbd.ByteWidth = sizeof(VertexFormat) * _numVerts;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			vbd.MiscFlags = 0;
			vbd.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = verts;
			HR(Renderer::theDevicePtr->CreateBuffer( &vbd, &vinitData, &vertexBufferPtr));
		}
		else
		{
			D3D11_BUFFER_DESC ibd;

			vertexBufferPtr->GetDesc(&ibd);

			D3D11_SUBRESOURCE_DATA iinitData;
			UINT oldBuffeSize = ibd.ByteWidth / sizeof(VertexFormat);
			UINT newBuffeSize = oldBuffeSize + _numVerts;

			iinitData.pSysMem = new VertexFormat[newBuffeSize];
			//memcpy((char *)(iinitData.pSysMem) + 100, 
			memcpy((char *)(iinitData.pSysMem) + ibd.ByteWidth, 
				verts, sizeof(VertexFormat) * _numVerts);

			//ZeroMemory((char *)(iinitData.pSysMem) + ibd.ByteWidth, sizeof(UINT) * _numIndices);

			ibd.ByteWidth += sizeof(VertexFormat) * _numVerts;
			//ibd.Usage = D3D11_USAGE_DEFAULT;
			//CComPtr<ID3D11Buffer> newIndexBufferPtr;
			ID3D11Buffer *newVertexBufferPtr;

			HR(Renderer::theDevicePtr->CreateBuffer(&ibd, &iinitData, &newVertexBufferPtr));

			SetD3DName(vertexBufferPtr, "The Index Buffer");

			Renderer::theContextPtr->CopySubresourceRegion(newVertexBufferPtr, 0, 0, 0, 0,
				vertexBufferPtr, 0, 0);

			ReleaseCOM(vertexBufferPtr);
			vertexBufferPtr = newVertexBufferPtr;
			//newIndexBufferPtr.CopyTo(&indexBufferPtr.p);

			delete [] iinitData.pSysMem;
			ret = oldBuffeSize;
		}
		return (UINT)ret;
	}
}