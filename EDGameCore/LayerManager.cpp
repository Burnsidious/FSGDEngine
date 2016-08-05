#include "LayerManager.h"
#include "UnorderedPair.h"
#include <set>
#include <assert.h>
#include <string>

namespace EDGameCore
{
	RegistrationId LayerManager::layers[32] = {0};
	unsigned int LayerManager::interactionSet[16] = {0};

	int LayerManager::GetLayerIndex(RegistrationId id)
	{
		for(int i = 0; i < 32; ++i)
		{
			if( layers[i] == id )
				return i;
		}

		return -1;	
	}

	unsigned int LayerManager::GetLayerMask(RegistrationId id)
	{
		for(unsigned int i = 0; i < 32; ++i)
		{
			if( layers[i] == id )
				return (1 << i);
		}

		return 0;
	}

	unsigned int LayerManager::GetLayerMask(const char* identifier)
	{
		RegistrationId id = (unsigned int)std::hash<std::string>()(identifier);

		return GetLayerMask(id);
	}

	void LayerManager::SetLayers(RegistrationId lrs[32])
	{
		memcpy_s( layers, sizeof(RegistrationId) * 32, lrs, sizeof(RegistrationId) * 32 );
	}

	void LayerManager::SetLayer(unsigned int index, RegistrationId id)
	{
		assert( index < 32 );

		layers[index] = id;
	}

	void LayerManager::AddLayerInteractionSet(RegistrationId layer1, RegistrationId layer2)
	{
		if( layer1 != layer2 )
		{
			int index0 = GetLayerIndex(layer1);
				
			if( index0 == -1 )
				return;

			int index1 = GetLayerIndex(layer2);

			if( index1 == -1 )
				return;

			int min = index0;
			int max = index1;

			if( max < min )
			{
				min = index1;
				max = index0;
			}

			int bitindex = min * (64 - min - 3) / 2 + max - 1;
			int mask = 1 << (bitindex & 31);

			interactionSet[ bitindex >> 5 ] |= mask;
		}
	}

	void LayerManager::RemoveLayerIntersectionSet(RegistrationId layer1, RegistrationId layer2)
	{
		if( layer1 != layer2 )
		{
			int index0 = GetLayerIndex(layer1);
				
			if( index0 == -1 )
				return;

			int index1 = GetLayerIndex(layer2);

			if( index1 == -1 )
				return;

			int min = index0;
			int max = index1;

			if( max < min )
			{
				min = index1;
				max = index0;
			}

			int bitindex = min * (64 - min - 3) / 2 + max - 1;
			int mask = 1 << (bitindex & 31);

			interactionSet[ bitindex >> 5 ] &= ~mask;
		}
	}

	bool LayerManager::LayersInteract(RegistrationId layer1, RegistrationId layer2)
	{
		if( layer1 == layer2 )
			return true;

		int index0 = GetLayerIndex(layer1);
				
		if( index0 == -1 )
			return false;

		int index1 = GetLayerIndex(layer2);

		if( index1 == -1 )
			return false;

		return LayersInteract( index0, index1 );
	}

	bool LayerManager::LayersInteract(int layerIndex1, int layerIndex2)
	{
		if( layerIndex1 == layerIndex2 )
			return true;

		int min = layerIndex1;
		int max = layerIndex2;

		if( max < min )
		{
			min = layerIndex2;
			max = layerIndex1;
		}

		int bitindex = min * (64 - min - 3) / 2 + max - 1;
		int mask = 1 << (bitindex & 31);

		return (interactionSet[ bitindex >> 5 ] & mask) != 0;		
	}
}