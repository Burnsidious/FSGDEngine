#pragma once

#include "DynAabbTree.h"
#include "GameObject.h"
#include "DynAabbTreeNode.h"



namespace EDGOCA
{
	class Scene
	{
		DynAabbTree dynamicObjectTree;
		DynAabbTree staticObjectTree;

		DynAabbTree visTree;

		std::list<GameObject*> updateables;
		std::map<unsigned int, GameObject*> uniques;
		std::list<GameObject*> deletingObjects;
		std::set<GameObject*> masterObjectSet;

		unsigned int sceneFlags;

	public:

		enum SceneFlag{ SF_OCCLUSION_CULL = 1 }; 

		Scene(void) : sceneFlags(0) {}
		~Scene();

		GameObject* SpawnObject(const char* xmlFileNamePtr, const EDMath::Float4x4* xform = 0);
		void DespawnObject(GameObject* object);

		GameObject* GetUniqueObject(unsigned int id);

		void Update(void);

		//void FrustumCull( EDCollision::FrustumHull* frustum, IMessage* msg );

		void SetObjectRenderableState(GameObject* object, bool state);

		void SetOcclusionQueryCreator(IOcclusionQueryCreator creator);

		unsigned int GetSceneFlags(void){ return sceneFlags; }
		void EnableSceneFlags( unsigned int flags ){ sceneFlags |= flags; }
		void DisableSceneFlags( unsigned int flags ){ sceneFlags &= ~flags; }

/*		void ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Segment& segment, void* voidPtr = 0 );
		void ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr = 0 );
		void ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Aabb& aabb, void* voidPtr = 0 );
		void ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::FrustumHull& frustum, void* voidPtr = 0 );		

		void ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Segment& segment, void* voidPtr = 0 );
		void ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr = 0 );
		void ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Aabb& aabb, void* voidPtr = 0 );
		void ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::FrustumHull& frustum, void* voidPtr = 0 );	*/	
	};
}