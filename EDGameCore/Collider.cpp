#include "Collider.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Game.h"
#include "UnorderedPair.h"
#include "LayerManager.h"

#include "../EDCollision/EDCollision.h"
#include "../EDCollision/sphere.h"
#include "../EDCollision/capsule.h"
#include "../EDCollision/obb.h"
#include "../EDCollision/EDBVH.h"
#include "../EDCollision/triangle.h"
#include "../EDCollision/segment.h"
#include "../EDCollision/ray.h"
#include "../EDCollision/aabb.h"

#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "CharacterCollider.h"
#include "MeshCollider.h"
#include "WheelCollider.h"

#include "../EDMath/polygon.h"

#include "../tinyxml/tinyxml.h"

namespace EDGameCore
{
	ICollider::ICollider(const ICollider& rhs)
	{
		collidableTreeLeaf = 0;
		containingTree = 0;
		trigger = rhs.trigger;
		materialId = rhs.materialId;
	}

	ICollider::~ICollider(void)
	{
		if( containingTree != 0 )
			containingTree->Remove( collidableTreeLeaf );

		collidableTreeLeaf = 0;
		containingTree = 0;

		while( !contactList.empty() )
		{
			ICollider* other = contactList.front();

			auto collisionMapIter = Physics::collisionContactMap.find( UnorderedPair<ICollider*>(this, other) );

			this->contactList.remove(other);
			other->contactList.remove(this);

			Physics::collisionContactMap.erase( collisionMapIter );
		}

		GameObject* gameObject = GetGameObject();
		if( gameObject && gameObject->GetCollider() == this )
			gameObject->SetCollider(RegistrationId(0));
	}

	bool ICollider::Collide(ICollider* other, EDCollision::Contact* contact)
	{
		ColliderType otherType = other->GetColliderType();

		switch( otherType )
		{
		case SPHERE:
				return Collide( ((SphereCollider*)other)->GetWorldSphere(), contact );
		case CAPSULE:
				return Collide( ((CapsuleCollider*)other)->GetWorldCapsule(), contact );
		case BOX:
				return Collide( ((BoxCollider*)other)->GetWorldBox(), contact );
		case CHARACTER:
				return Collide( ((CharacterCollider*)other)->GetWorldCapsule(), contact );
		case WHEEL:
		case MESH:
				return other->Collide( this, contact );
		default:
			break;
		}

		return false;
	}

	bool ICollider::Collide(const BoundingVolume& boundingVolume, EDCollision::Contact* contact)
	{
		ColliderType thisType = GetColliderType();

		switch( thisType )
		{
		case SPHERE:
				return EDCollision::Collide( &((SphereCollider*)this)->GetWorldSphere(), &boundingVolume, contact );
		case CAPSULE:
				return EDCollision::Collide( &((CapsuleCollider*)this)->GetWorldCapsule(), &boundingVolume, contact );
		case BOX:
				return EDCollision::Collide( &((BoxCollider*)this)->GetWorldBox(), &boundingVolume, contact );
		case CHARACTER:
				return EDCollision::Collide( &((CharacterCollider*)this)->GetWorldCapsule(), &boundingVolume, contact );
		default:
			break;
		}

		return false;
	}

	void ICollider::OnDestroy(void)
	{
	}

	RigidBody* ICollider::GetAttachedRigidBody(void)
	{
		GameObject* gameObject = GetGameObject();
		RigidBody* attachedBody = gameObject->GetRigidBody();

		if( attachedBody == 0 )
		{
			Transform* parentTransform = gameObject->GetTransform()->GetParent();
			while( parentTransform != 0 )
			{
				attachedBody = parentTransform->GetGameObject()->GetRigidBody();

				if( attachedBody != 0 )
					break;

				parentTransform = parentTransform->GetParent();
			}
		}

		return attachedBody;
	}

	const EDPhysics::PhysicsMaterial& ICollider::GetMaterial(void)const
	{
		return EDPhysics::GetPhysicsMaterial(materialId);
	}

	ColliderCombination ICollider::GetColliderCombination(void)
	{
		RigidBody* attachedRigidBody = GetAttachedRigidBody();

		if( attachedRigidBody != 0 )
		{
			if( attachedRigidBody->IsKinematic() )
			{
				if( isTrigger() ) // Kinematic RigidBody Trigger Collider
				{
					// Actively tests for overlap against:
					//	Every collider

					return KRTC;
				}
				else // Kinematic RigidBody Collider
				{
					// Actively tests for overlap against:
					// Static trigger colliders
					///	All trigger colliders

					return KRC;
				}
			}
			else 
			{
				if( isTrigger() ) // RigidBody Trigger Collider
				{
					// Actively tests for overlap against:
					//	Every collider

					return RTC;
				}
				//else // RigidBody Collider
				//{
				//	// Actively tests for collisions against:
				//	//	Static Colliders, RigidBody Colliders, Kinematic RigidBody Colliders

				//	// Actively tests for overlap against:
				//	//  Static trigger colliders
				//	///	All trigger colliders

				//	return RC;
				//}
				else if( attachedRigidBody->IsSleeping() )
				{
					const uint8_t SLEEP_DELAY_LIMIT = 128;

					if( attachedRigidBody->sleepDelayCounter < SLEEP_DELAY_LIMIT )
					{
						attachedRigidBody->sleepDelayCounter++;
						return RC;
					}

					return RCS;
				}
				else
				{
					attachedRigidBody->sleepDelayCounter = 0;
					return RC;
				}
			}
		}
		else
		{
			if( isTrigger() ) // Static trigger collider
			{
				return STC;
			}
			else // Static Collider
			{
				return SC;
			}
		}		
	}

	unsigned int ICollider::CollidersInteract(ICollider* colliderOne, ICollider* colliderTwo)
	{
		if( colliderOne->isEnabledInHierarchy() == false || colliderTwo->isEnabledInHierarchy() == false )
			return 0;

		ColliderCombination combo1 = colliderOne->GetColliderCombination();
		ColliderCombination combo2 = colliderTwo->GetColliderCombination();

		return (1 << combo1) | (1 << combo2);
	}

	EDMath::Aabb ICollider::GetAabb(ICollider& c)
	{
		return c.GetAabb();
	}

	void ICollider::WakeContacts(void)
	{
		std::list<ICollider*>::iterator colliderIter = contactList.begin();
		std::list<ICollider*>::iterator colliderEnd = contactList.end();
	
		for(; colliderIter != colliderEnd; ++colliderIter)
		{
			RigidBody* rigidBody = (*colliderIter)->GetAttachedRigidBody();

			if( rigidBody != 0 )
				rigidBody->Wake();
		}
	}
}