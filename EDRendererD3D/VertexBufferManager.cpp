#include "precompiled.h"
#include "VertexBufferManager.h"
#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;

namespace EDRendererD3D
{
	VertexBufferManager *VertexBufferManager::instancePtr = 0;

	VertexBufferManager::VertexBufferManager(void)
	{

 	}


	VertexBufferManager::~VertexBufferManager(void)
	{
	}

	VertexBufferManager &VertexBufferManager::GetReference()
	{
		if(0 == instancePtr)
		{
			instancePtr = new VertexBufferManager;
		}
		return *instancePtr;
	}

	void VertexBufferManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = 0;
	}

	template <>
	VertexBuffer<VERTEX_POS> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPositionBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSCOLOR> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPositionColorBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSTEX> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPositionTexBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSNORMTEX> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPosNormTexBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSNORMTANTEX> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPosNormTanTexBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSBONEWEIGHT> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPosBoneWeightBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSBONEWEIGHTNORMTEX> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPosBoneWeightNormTexBuffer();
	}

	template <>
	VertexBuffer<VERTEX_POSBONEWEIGHTNORMTANTEX> &VertexBufferManager::GetVertexBuffer()
	{
		return GetPosBoneWeightNormTanTexBuffer();
	}
}