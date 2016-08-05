#pragma once
#include "../EDMath/float3.h"
#include "../EDMath/capsule.h"
#include "../EDSpatial/DynAabbTree.h"
#include "UnorderedPair.h"
#include "../EDCollision/EDCollision.h"
#include "ColliderEnums.h"
#include <set>

using namespace EDMath;

namespace EDGameCore
{
	class GameObject;

	struct RayHit;

	class Physics
	{
		friend class ICollider;

		class ColliderTree : public EDSpatial::DynAabbTreeT<ICollider*>{};

		static ColliderTree colliderTree[][CC_COUNT];

	public:

		class traverser : public EDSpatial::DynAabbTree::traverser<ICollider*>
		{
		private:
			unsigned int layerMask;
			
			void onObject(ICollider* collider);

		public:
			virtual ~traverser(){}

			virtual void onCollider(ICollider* collider) = 0;

			void traverse(void);

			unsigned int GetLayerMask(void){ return layerMask; }
			void SetLayerMask(unsigned int m){ layerMask = m; }
		};

		class Collision
		{
			friend class Physics;
		private:
			ICollider* firstCollider;
			ICollider* secondCollider;
			EDCollision::Contact contact;
			int coherenceCount;

		public:
			Collision(void) : firstCollider(0), secondCollider(0), coherenceCount(0) {}
			
			const ICollider* GetFirstCollider(void)const{ return firstCollider; }
			const ICollider* GetSecondCollider(void)const{ return secondCollider; }

			const EDCollision::Contact& GetContact(void)const{ return contact; }
		};

		static bool RayCast(Float3 rayStart, Float3 normal, float maxDistance, unsigned int layerMask, RayHit& hit);

		template<typename Func>
		static void OverlapBoundingVolume(const BoundingVolume& bv, Func func, unsigned int layerMask = 0xFFFFFFFF);
		
	protected:
		Physics(void){}
		Physics(const Physics&){}
		Physics& operator=(const Physics&){return *this;}
		virtual ~Physics(){}

		static void FixedUpdate(GameObject* obj);
		static void Update(GameObject* obj);
		static void ProcessCollisions(void);

		static map<UnorderedPair<ICollider*>, Collision> collisionContactMap;

		static bool GenerateContact( ICollider* colliderA, ICollider* colliderB, const Float3& a2bOffset, EDCollision::Contact& contact );
		static void AddContact(ICollider* colliderA, ICollider* colliderB, const EDCollision::Contact& contact);
		
		static void ProcessCollisionMap(void);

		static void ResolveRCtoRC(void* a, void* b, const Float3& a2bOffset, void* voidPtr);
		static void ResolveRCtoKRC(void* a, void* b, const Float3& a2bOffset, void* voidPtr);
		static void ResolveRCtoSC(void* a, void* b, const Float3& a2bOffset, void* voidPtr);
		static void ResolveTriggerOverlap(void* a, void* b, const Float3& a2bOffset, void* voidPtr);

		template<typename Func>
		static void OverlapBoundingVolume(const BoundingVolume& bv, const Aabb& bvAabb, EDSpatial::DynAabbTreeT<ICollider*>::Node* node, Func func);
	};

	template<typename Func>
	void Physics::OverlapBoundingVolume(const BoundingVolume& bv, Func func, unsigned int layerMask)
	{
		Aabb boundsAabb = bv.GetAabb();

		for(unsigned int j = 0; j < 32; ++j)
		{
			if( (layerMask & (1<<j)) == 0 )
				continue;

			for(unsigned int i = RC; i < STC; ++i)
			{
				if( colliderTree[j][i].GetRoot() == 0 )
					continue;
				
				OverlapBoundingVolume( bv, boundsAabb, colliderTree[j][i].GetRoot(), func );
			}
		}
	}

	template<typename Func>
	void Physics::OverlapBoundingVolume(const BoundingVolume& bv, const Aabb& bvAabb, EDSpatial::DynAabbTreeT<ICollider*>::Node* node, Func func)
	{
		if( !EDCollision::AabbToAabb( bvAabb, node->GetAabb() ) )
			return;

		auto* leaf = (EDSpatial::DynAabbTreeT<ICollider*>::Leaf*)node->asLeaf();
		auto* branch = (EDSpatial::DynAabbTreeT<ICollider*>::Branch*)node->asBranch();

		if( leaf != 0 )
		{
			ICollider* collider = (ICollider*)leaf->GetObjectPtr();
			
			if( collider->Collide( bv ) )
				func( collider );

			return;
		}

		OverlapBoundingVolume( bv, bvAabb, branch->leftChild, func );
		OverlapBoundingVolume( bv, bvAabb, branch->rightChild, func );
	}
}