#pragma once

#include "Component.h"
#include "GameObject.h"
#include "../EDMath/aabb.h"
#include "../EDMath/sphere.h"
#include "../EDMath/capsule.h"
#include "../EDMath/obb.h"
#include "../EDSpatial/DynAabbTree.h"
#include "../EDCollision/EDCollision.h"
#include "../EDCollision/EDBVH.h"
#include "../EDPhysics/EDPhysics.h"
#include <list>
#include "../EDUtilities/ContentManager.h"
#include "Physics.h"
#include "ColliderEnums.h"

using namespace EDUtilities;

namespace EDGameCore
{
	class RigidBody;
	struct RayHit;
	class SphereCollider;
	class CapsuleCollider;
	class BoxCollider;
	class CharacterCollider;
	class WheelCollider;
	class IMeshCollider;

	static const unsigned int INVALID_TRI_INDEX = 0xFFFFFFFF;

	class ICollider : public Component
	{
		friend class Game;
		friend class GameObject;
		friend class Physics;

	public:

		ICollider(void) : collidableTreeLeaf(0), containingTree(0), trigger(false), materialId(EDPhysics::MATERIAL_DEFAULT){}
		ICollider(const ICollider& rhs);

		virtual ~ICollider(void);

		ComponentType GetComponentType(void)const{ return COLLIDER; }

		virtual const EDMath::Aabb& GetAabb(void) = 0;
		
		static EDMath::Aabb GetAabb(ICollider& c);
		
		bool isTrigger(void){ return trigger; }
		void SetTrigger(bool state){ trigger = state; }

		RigidBody* GetAttachedRigidBody(void);

		virtual ColliderType GetColliderType(void)const{ return INVALID; }

		static unsigned int CollidersInteract(ICollider* colliderOne, ICollider* colliderTwo);

		EDPhysics::MaterialId GetMaterialId(void)const{ return materialId; }
		void SetMaterialId(EDPhysics::MaterialId mId){ materialId = mId; }

		const EDPhysics::PhysicsMaterial& GetMaterial(void)const;

		ColliderCombination GetColliderCombination(void);

		ICollider* clone(void)const = 0;
		
		virtual bool Collide(ICollider* other, EDCollision::Contact* contact = 0);
		virtual bool Collide(const BoundingVolume& boundingVolume, EDCollision::Contact* contact = 0);

		virtual bool OverlapRay(Float3 start, Float3 direction, float maxLength = FLT_MAX){ return false; }
		virtual bool IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength = FLT_MAX){ return false; }

	private:
		
		EDSpatial::DynAabbTree::LeafT<ICollider*>* collidableTreeLeaf;
		EDSpatial::DynAabbTreeT<ICollider*>* containingTree;
		
		std::list<ICollider*> contactList;

		bool trigger;
		EDPhysics::MaterialId materialId;

		void WakeContacts(void);

	protected:
		Float3 lastPosition;
		void OnDestroy(void);
	};

	template<typename Derived>
	class ColliderT : public ICollider
	{
		friend class Game;
		friend class Registry;
	public:
		virtual ~ColliderT(void){}

		void LoadState(TiXmlElement* xmlElement){}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }

	protected:
		EDMath::Aabb worldAabb;

	private:
		ICollider* clone(void)const
		{
			return new Derived(*(Derived*)this);
		}

		static RegistrationId typeId;
		static string typeName;
	};

	template<typename Derived>
	RegistrationId ColliderT<Derived>::typeId = 0;
	
	template<typename Derived>
	string ColliderT<Derived>::typeName;

	struct RayHit
	{
		ICollider* collider;
		
		float distance;
		Float3 Point;
		Float3 Normal;
		Float3 Barycentric;
		unsigned int triIndex;

		RayHit(void) : collider(0), distance(FLT_MAX), triIndex(INVALID_TRI_INDEX) {}
	};
}