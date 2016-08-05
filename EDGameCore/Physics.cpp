#include "Physics.h"
#include "Game.h"
#include "GameObject.h"
#include "../EDSpatial/DynAabbTree.h"
#include "GameObject.h"
#include "Collider.h"
#include "RigidBody.h"
#include "Transform.h"
#include "../EDCollision/Ray.h"
#include "../EDCollision/sphere.h"
#include "../EDCollision/obb.h"
#include "../EDCollision/capsule.h"
#include "LayerManager.h"
#include "WheelCollider.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "CharacterCollider.h"

#define PENETRATION_MIN 0.01f

namespace EDGameCore
{
	Physics::ColliderTree Physics::colliderTree[32][CC_COUNT];

	map<UnorderedPair<ICollider*>, Physics::Collision> Physics::collisionContactMap;

	namespace
	{
		class RayCastTraverser : public Physics::traverser
		{
		public:
			Float3 rayStart;
			Float3 normal;
			float maxDistance;
			bool hasHit;
			RayHit* hit;

			bool testAabb(const Aabb& aabb)const
			{
				return EDCollision::OverlapRayAabb( rayStart, normal, maxDistance, aabb );
			}

			void onCollider(ICollider* collider)
			{
				RayHit rayHit;

				if( collider->IntersectRay( rayStart, normal, rayHit, maxDistance ) )
				{
					if( rayHit.distance < hit->distance )
					{
						hasHit = true;
						maxDistance = rayHit.distance;
						*hit = rayHit;
						hit->collider = collider;
					}
				}
			}
		};
	}

	void Physics::traverser::onObject(ICollider* collider)
	{
		if( collider->isEnabled() && collider->GetGameObject()->isEnabledInHierarchy() )
			onCollider(collider);
	}

	void Physics::traverser::traverse(void)
	{
		unsigned int layerMask = GetLayerMask();

		for(unsigned int l = 0; l < 32; ++l)
		{
			if( layerMask & (1<<l) )
			{
				for(unsigned int i = 0; i < STC; ++i)
					EDSpatial::DynAabbTree::traverser<ICollider*>::traverse( &colliderTree[l][i] );
			}
		}
	}

	void Physics::FixedUpdate(GameObject* obj)
	{
		if( obj->isEnabledInHierarchy() )
			obj->FixedUpdate();
	}

	void Physics::Update(GameObject* obj)
	{
		ICollider* collider = obj->GetCollider();

		if( collider != 0 )
		{
			ColliderCombination colliderCombination = collider->GetColliderCombination();

			if( colliderCombination == KRC && obj->GetTransform()->wasModified() )
			{
				collider->WakeContacts();	
			}

			int layerIndex = Game::GetLayerIndex( collider->GetGameObject()->GetLayer() );

			if( collider->containingTree != &colliderTree[layerIndex][colliderCombination] )
			{
				if( collider->containingTree != 0 )
					collider->containingTree->Remove( collider->collidableTreeLeaf );

				collider->collidableTreeLeaf = colliderTree[layerIndex][colliderCombination].Insert( collider );
				collider->containingTree = &colliderTree[layerIndex][colliderCombination];
			}
			else
				collider->containingTree->Update( collider->collidableTreeLeaf );
		}
	}

	void Physics::ProcessCollisions(void)
	{
		for(int i = 0; i < 32; ++i)
		{
			for(int j = i; j < 32; ++j)
			{
				if( Game::LayersInteract( i, j ) )
				{
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][SC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveRCtoSC, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][KRC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveRCtoKRC, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][RCS], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveRCtoRC, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][RC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveRCtoRC, 0 );


					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][SC], &colliderTree[j][RTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][SC], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );

					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][STC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][RTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RC], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );

					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RCS], &colliderTree[j][STC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RCS], &colliderTree[j][RTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RCS], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );

					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][KRC], &colliderTree[j][STC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][KRC], &colliderTree[j][RTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][KRC], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );

					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][STC], &colliderTree[j][RTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][STC], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );

					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RTC], &colliderTree[j][RTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][RTC], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );

					EDSpatial::DynAabbTree::TreeTreeOverlap( &colliderTree[i][KRTC], &colliderTree[j][KRTC], Float3(0.0f, 0.0f, 0.0f), Physics::ResolveTriggerOverlap, 0 );
				}
			}
		}

		Physics::ProcessCollisionMap();
	}

	bool Physics::RayCast(Float3 rayStart, Float3 normal, float maxDistance, unsigned int layerMask, RayHit& hit)
	{
		hit.distance = FLT_MAX;
		hit.collider = nullptr;

		RayCastTraverser rayCast;
		rayCast.rayStart = rayStart;
		rayCast.normal = normal;
		rayCast.maxDistance = maxDistance;
		rayCast.hasHit = false;
		rayCast.SetLayerMask(layerMask);
		rayCast.hit = &hit;

		rayCast.traverse();

		return rayCast.hasHit;
	}

	bool Physics::GenerateContact( ICollider* colliderA, ICollider* colliderB, const Float3& a2bOffset, EDCollision::Contact& contact )
	{
		if( colliderA->Collide( colliderB, &contact ) )
		{
			if( contact.PenetrationDepth > PENETRATION_MIN )
			{
				contact.PenetrationDepth -= PENETRATION_MIN;
				AddContact( colliderA, colliderB, contact );
				return true;
			}
		}

		return false;
	}

	void Physics::AddContact(ICollider* colliderA, ICollider* colliderB, const EDCollision::Contact& contact)
	{
		UnorderedPair<ICollider*> collisionPair(colliderA, colliderB);

		auto collisionMapIter =	collisionContactMap.find( collisionPair );

		if( collisionMapIter == collisionContactMap.end() )
		{
			Collision toAdd;
			toAdd.coherenceCount = 1;
			toAdd.firstCollider = colliderA;
			toAdd.secondCollider = colliderB;
			toAdd.contact = contact;

			collisionContactMap.insert( std::make_pair( collisionPair, toAdd ) );

			colliderA->contactList.push_back(colliderB);
			colliderB->contactList.push_back(colliderA);
		}
		else
		{
			collisionMapIter->second.coherenceCount = 2;
			collisionMapIter->second.contact = contact;
		}
	}

	void Physics::ProcessCollisionMap(void)
	{
		auto collisionMapIter = collisionContactMap.begin();
		auto collisionMapEnd = collisionContactMap.end();

		for(; collisionMapIter != collisionMapEnd;)
		{
			ICollider* colliderA = collisionMapIter->second.firstCollider;
			ICollider* colliderB = collisionMapIter->second.secondCollider;

			RigidBody* rigidBodyA = colliderA->GetAttachedRigidBody();
			RigidBody* rigidBodyB = colliderB->GetAttachedRigidBody();

			auto& collision = collisionMapIter->second;
			auto& contact = collision.contact;
			auto flippedContact = contact;
			flippedContact.negate();

			if( collision.coherenceCount == 0 )
			{
				auto toRemove = collisionMapIter;
				
				if( colliderA->isTrigger() || colliderB->isTrigger() )
				{
					colliderA->GetGameObject()->OnTriggerExit( colliderA, colliderB, contact );
					if( rigidBodyA != 0 && rigidBodyA->GetGameObject() != colliderA->GetGameObject() )
						rigidBodyA->GetGameObject()->OnTriggerExit( colliderA, colliderB, contact );

					colliderB->GetGameObject()->OnTriggerExit( colliderB, colliderA, flippedContact );
					if( rigidBodyB != 0 && rigidBodyB->GetGameObject() != colliderB->GetGameObject() )
						rigidBodyB->GetGameObject()->OnTriggerExit( colliderB, colliderA, flippedContact );
				}

				colliderA->GetGameObject()->OnCollisionExit( colliderA, colliderB, contact );
				if( rigidBodyA != 0 && rigidBodyA->GetGameObject() != colliderA->GetGameObject() )
					rigidBodyA->GetGameObject()->OnCollisionExit( colliderA, colliderB, contact );

				colliderB->GetGameObject()->OnCollisionExit( colliderB, colliderA, flippedContact );
				if( rigidBodyB != 0 && rigidBodyB->GetGameObject() != colliderB->GetGameObject() )
					rigidBodyB->GetGameObject()->OnCollisionExit( colliderB, colliderA, flippedContact );

				colliderA->contactList.remove(colliderB);
				colliderB->contactList.remove(colliderA);

				collisionMapIter++;
				collisionContactMap.erase(toRemove);
			}
			else if( collision.coherenceCount == 1 )
			{

				if( colliderA->isTrigger() || colliderB->isTrigger() )
				{
					colliderA->GetGameObject()->OnTriggerEnter( colliderA, colliderB, contact );
					if( rigidBodyA != 0 && rigidBodyA->GetGameObject() != colliderA->GetGameObject() )
						rigidBodyA->GetGameObject()->OnTriggerEnter( colliderA, colliderB, contact );

					colliderB->GetGameObject()->OnTriggerEnter( colliderB, colliderA, flippedContact );
					if( rigidBodyB != 0 && rigidBodyB->GetGameObject() != colliderB->GetGameObject() )
						rigidBodyB->GetGameObject()->OnTriggerEnter( colliderB, colliderA, flippedContact );
				}

				colliderA->GetGameObject()->OnCollisionEnter( colliderA, colliderB, contact );
				if( rigidBodyA != 0 && rigidBodyA->GetGameObject() != colliderA->GetGameObject() )
					rigidBodyA->GetGameObject()->OnCollisionEnter( colliderA, colliderB, contact );

				colliderB->GetGameObject()->OnCollisionEnter( colliderB, colliderA, flippedContact );
				if( rigidBodyB != 0 && rigidBodyB->GetGameObject() != colliderB->GetGameObject() )
					rigidBodyB->GetGameObject()->OnCollisionEnter( colliderB, colliderA, flippedContact );

				collisionMapIter->second.coherenceCount = 0;
				collisionMapIter++;
			}
			else if( collision.coherenceCount > 1 )
			{
				if( colliderA->isTrigger() || colliderB->isTrigger() )
				{
					colliderA->GetGameObject()->OnTriggerStay( colliderA, colliderB, contact );
					if( rigidBodyA != 0 && rigidBodyA->GetGameObject() != colliderA->GetGameObject() )
						rigidBodyA->GetGameObject()->OnTriggerStay( colliderA, colliderB, contact );

					colliderB->GetGameObject()->OnTriggerStay( colliderB, colliderA, flippedContact );
					if( rigidBodyB != 0 && rigidBodyB->GetGameObject() != colliderB->GetGameObject() )
						rigidBodyB->GetGameObject()->OnTriggerStay( colliderB, colliderA, flippedContact );
				}

				colliderA->GetGameObject()->OnCollisionStay( colliderA, colliderB, contact );
				if( rigidBodyA != 0 && rigidBodyA->GetGameObject() != colliderA->GetGameObject() )
					rigidBodyA->GetGameObject()->OnCollisionStay( colliderA, colliderB, contact );

				colliderB->GetGameObject()->OnCollisionStay( colliderB, colliderA, flippedContact );
				if( rigidBodyB != 0 && rigidBodyB->GetGameObject() != colliderB->GetGameObject() )
					rigidBodyB->GetGameObject()->OnCollisionStay( colliderB, colliderA, flippedContact );

				collisionMapIter->second.coherenceCount = 0;
				collisionMapIter++;
			}
		}
	}

	void Physics::ResolveRCtoRC(void* a, void* b, const Float3& a2bOffset, void* voidPtr)
	{
		ICollider* colliderA = (ICollider*)a;
		ICollider* colliderB = (ICollider*)b;
		
		if( ICollider::CollidersInteract( colliderA, colliderB ) )
		{
			RigidBody* rigidBodyA = colliderA->GetAttachedRigidBody();
			RigidBody* rigidBodyB = colliderB->GetAttachedRigidBody();

			if( rigidBodyA == rigidBodyB )
				return;

			if( rigidBodyA->IsSleeping() && rigidBodyB->IsSleeping() )
				return;

			/**/
			EDMath::Aabb worldAabbA = colliderA->GetAabb();

			Float3 deltaA = worldAabbA.center - colliderA->lastPosition;
			
			EDMath::Aabb worldAabbB = colliderB->GetAabb();

			Float3 deltaB = worldAabbB.center - colliderB->lastPosition;

			Float3 delta = deltaA - deltaB;

			int count = 1;
			float ratio = 1.0f;

#ifndef DEBUG
			if( delta.squaredMagnitude() > ED_EPSILON )
			{
				float mag = delta.magnitude();
				Float3 norm = delta * (1.0f/mag);
				
				float projR = worldAabbA.extents.x * abs(norm.x) + 
					worldAabbA.extents.y * abs(norm.y) +
					worldAabbA.extents.z * abs(norm.z);
				
				projR += worldAabbB.extents.x * abs(norm.x) + 
						worldAabbB.extents.y * abs(norm.y) +
						worldAabbB.extents.z * abs(norm.z);

				projR *= 0.25f;

				count = int(mag / projR) + 1;
				ratio = 1.0f / count;
			}
#endif

			Float4x4 xformA = rigidBodyA->GetTransform()->GetWorldMatrix();
			xformA.WAxis -= deltaA;

			Float4x4 xformB = rigidBodyB->GetTransform()->GetWorldMatrix();
			xformB.WAxis -= deltaB;

			for(int i = 0; i < count; ++i)
			{
				xformA.WAxis += deltaA * ratio;

				rigidBodyA->CenterAt(xformA);

				xformB.WAxis += deltaB * ratio;

				rigidBodyB->CenterAt(xformB);

				if( ! EDCollision::AabbToAabb( colliderA->GetAabb(), colliderB->GetAabb() ) )
					continue;

				EDCollision::Contact contact;

				if( Physics::GenerateContact( colliderA, colliderB, Float3(0.0f, 0.0f, 0.0f), contact ) )
				{
					colliderA->lastPosition = colliderA->GetAabb().center;
					colliderB->lastPosition = colliderB->GetAabb().center;

					if( colliderA->GetColliderType() == WHEEL ||
						colliderB->GetColliderType() == WHEEL )
					{				
					}
					else
						RigidBody::ResolveCollision(rigidBodyA, rigidBodyB, contact, colliderA->GetMaterialId(), colliderB->GetMaterialId() );
				
					
					return;
				}				
			}
		}
	}

	void Physics::ResolveRCtoKRC(void* a, void* b, const Float3& a2bOffset, void* voidPtr)
	{
		ICollider* colliderA = (ICollider*)a;
		ICollider* colliderB = (ICollider*)b;
		
		if( ICollider::CollidersInteract( colliderA, colliderB ) )
		{
			RigidBody* rigidBodyA = colliderA->GetAttachedRigidBody();
			RigidBody* rigidBodyB = colliderB->GetAttachedRigidBody();

			float prevMass = rigidBodyB->GetInverseMass();
		
			ResolveRCtoRC( a, b, a2bOffset, voidPtr );

			rigidBodyB->SetInverseMass( prevMass );
		}
	}

	void Physics::ResolveRCtoSC(void* a, void* b, const Float3& a2bOffset, void* voidPtr)
	{
		ICollider* colliderA = (ICollider*)a;
		ICollider* colliderB = (ICollider*)b;
		
		if( ICollider::CollidersInteract( colliderA, colliderB ) )
		{
			RigidBody* rigidBodyA = colliderA->GetAttachedRigidBody();

			if( rigidBodyA->IsSleeping() )
				return;

			EDMath::Aabb worldAabb = colliderA->GetAabb();

			Float3 delta = worldAabb.center - colliderA->lastPosition;
			
			int count = 1;
			float ratio = 1.0f;

#ifndef DEBUG
			if( delta.squaredMagnitude() > ED_EPSILON )
			{
				float mag = delta.magnitude();
				Float3 norm = delta * (1.0f/mag);
				
				float projR = worldAabb.extents.x * abs(norm.x) + 
					worldAabb.extents.y * abs(norm.y) +
					worldAabb.extents.z * abs(norm.z);
				
				projR *= 0.25f;

				count = int(mag / projR) + 1;
				ratio = 1.0f / count;
			}
#endif

			Float4x4 xform = rigidBodyA->GetTransform()->GetWorldMatrix();
			xform.WAxis -= delta;

			for(int i = 0; i < count; ++i)
			{
				xform.WAxis += delta * ratio;

				rigidBodyA->CenterAt(xform);

				EDCollision::Contact contact;

				if( Physics::GenerateContact( colliderA, colliderB, Float3(0.0f, 0.0f, 0.0f), contact ) )
				{
					colliderA->lastPosition = colliderA->GetAabb().center;

					if( colliderA->GetColliderType() == WHEEL )
					{				
						WheelCollider* wheelPtr = (WheelCollider*)colliderA;
						Float4x4 wheelMatrix;
						wheelPtr->GetWheelMatrix(wheelMatrix);

						float mt = wheelPtr->GetMotorTorque();
						mt /= wheelPtr->GetRadius();
						float oldMt = mt;
						RigidBody::ResolveTireCollision( rigidBodyA, contact, wheelPtr->GetSuspensionDistance(), wheelPtr->GetRadius(), mt, wheelMatrix );
					}
					else if( colliderA->GetColliderType() == CHARACTER )
					{
						CharacterCollider* charPtr = (CharacterCollider*)colliderA;
					
						RigidBody::ResolveCharacterCollision( rigidBodyA, contact, charPtr->GetStandY(), charPtr->GetStopY(), colliderA->GetMaterialId(), colliderB->GetMaterialId() );
					}
					else
						RigidBody::ResolveCollision(rigidBodyA, contact, colliderA->GetMaterialId(), colliderB->GetMaterialId() );
				
					
					
					return;
				}				
			}
		}
	}

	void Physics::ResolveTriggerOverlap(void* a, void* b, const Float3& a2bOffset, void* voidPtr)
	{
		ICollider* colliderA = (ICollider*)a;
		ICollider* colliderB = (ICollider*)b;
		
		if( ICollider::CollidersInteract( colliderA, colliderB ) )
		{
			EDCollision::Contact contact;

			Physics::GenerateContact( colliderA, colliderB, Float3(0.0f, 0.0f, 0.0f), contact );
		}
	}
}