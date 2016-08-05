#pragma once

namespace EDGameCore
{
	typedef unsigned int RegistrationId;

	class LayerManager
	{
	public:
		static unsigned int GetLayerMask(RegistrationId id);
		static unsigned int GetLayerMask(const char* identifier);
		static bool LayersInteract(RegistrationId layer1, RegistrationId layer2);
		static bool LayersInteract(int layerIndex1, int layerIndex2);
		static int GetLayerIndex(RegistrationId id);

	protected:
		LayerManager(void){}
		LayerManager(const LayerManager&){}
		LayerManager& operator=(const LayerManager&){ return *this; }

		static void SetLayers(RegistrationId layers[32]);
		static void SetLayer(unsigned int index, RegistrationId id);

		static void AddLayerInteractionSet(RegistrationId layer1, RegistrationId layer2);
		static void RemoveLayerIntersectionSet(RegistrationId layer1, RegistrationId layer2);

		static RegistrationId layers[];
		static unsigned int interactionSet[];
	};
}