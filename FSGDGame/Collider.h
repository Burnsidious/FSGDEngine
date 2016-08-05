#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDCollision/CollidableTypes.h"
#include "../EDTree/AabbBvh.h"
#include "../EDTree/AabbBvhLeafNode.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDUtilities/GDAttribute.h"
#include "../EDUtilities/GDAttributeTypes.h"
#include "CollisionContact.h"

namespace Behaviors
{
	struct BVHTraversalStruct;
	typedef bool (*BVHPerNodeAabbFunc)(const EDCollision::Aabb& nodeAabb, 
		BVHTraversalStruct &traversalStruct);
	typedef void (*BVHPerTriangleFunc)(const EDMath::Float3& v0, const EDMath::Float3& v1, 
		const EDMath::Float3& v2, const EDMath::Float3& n, BVHTraversalStruct &traversalStruct);

	struct BVHTraversalStruct
	{
		BVHPerNodeAabbFunc perNodeFunc;
		BVHPerTriangleFunc perTriangleFunc;
		void* dataPtr;

		BVHTraversalStruct(void)
		{
			perNodeFunc = 0;
			perTriangleFunc = 0;
			dataPtr = 0;
		}
	};

	class BVHTemplate
	{
		AabbBvh* aabbBvhPtr;
		EDUtilities::ContentHandle<GDAttribute<Point>> m_PointsHandle;
		EDUtilities::ContentHandle<GDAttribute<TriVertIndices>> indicesHandle;
		EDUtilities::ContentHandle<GDAttribute<TriNormal>> triNormalsHandle;
		
		void Traverse(const EDTreeNode* node, BVHTraversalStruct& traversalStruct);

	public:
		
		BVHTemplate(void) : aabbBvhPtr(0) {}
		~BVHTemplate() { delete aabbBvhPtr; }

		static BVHTemplate* Load(const char* pGDMeshFile, const char* indentifier = 0);

		void Traverse( BVHTraversalStruct& traversalStruct );

		void GetLocalAabb(EDCollision::Aabb& aabb)const;
	};

	class Collider : public EDGOCA::BehaviorT<Collider>
	{
	public:
		enum ColliderType{ INVALID = -1, AABB = 0, OBB, SPHERE, CAPSULE, MESH };

		class ColliderObject
		{
		public:
			virtual ~ColliderObject(){}
			virtual ColliderType GetColliderType(void) = 0;
			virtual void GetLocalAabb(EDCollision::Aabb& aabb) = 0;
			virtual void ReadXML(TiXmlElement* objectEle) = 0;
			virtual ColliderObject* clone(void) = 0;
		};

		class AabbColliderObject : public ColliderObject
		{
			EDCollision::Aabb localAabb;
		public:
			ColliderType GetColliderType(void){ return AABB; }
			void GetLocalAabb(EDCollision::Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			AabbColliderObject* clone(void) { return new AabbColliderObject(*this); }
		};

		class ObbColliderObject : public ColliderObject
		{
			EDCollision::Obb localObb;
		public:
			ColliderType GetColliderType(void){ return OBB; }
			void GetLocalAabb(EDCollision::Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			ObbColliderObject* clone(void) { return new ObbColliderObject(*this); }
		};

		class SphereColliderObject : public ColliderObject
		{
			EDCollision::Sphere localSphere;
		public:
			ColliderType GetColliderType(void){ return SPHERE; }
			void GetLocalAabb(EDCollision::Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			const EDCollision::Sphere& GetLocalSphere(void){ return localSphere; }
			SphereColliderObject* clone(void){ return new SphereColliderObject(*this); }
		};

		class CapsuleColliderObject : public ColliderObject
		{
			EDCollision::Capsule localCapsule;
		public:
			ColliderType GetColliderType(void){ return CAPSULE; }
			void GetLocalAabb(EDCollision::Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			CapsuleColliderObject* clone(void){ return new CapsuleColliderObject(*this); }
		};

		class MeshColliderObject : public ColliderObject
		{
			EDUtilities::ContentHandle<BVHTemplate> bvhHandle;
		public:
			ColliderType GetColliderType(void){ return MESH; }
			void GetLocalAabb(EDCollision::Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);

			void Traverse(BVHTraversalStruct& traversalStruct);
			MeshColliderObject* clone(void){ return new MeshColliderObject(*this); }
		};
	
		static bool ColliderAabbToAabb(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderAabbToObb(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderAabbToSphere(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderAabbToCapsule(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderAabbToMesh(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);

		static bool ColliderObbToObb(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderObbToSphere(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderObbToCapsule(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderObbToMesh(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		
		static bool ColliderSphereToSphere(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderSphereToCapsule(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderSphereToMesh(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);

		static bool ColliderCapsuleToCapsule(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);
		static bool ColliderCapsuleToMesh(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);

		static bool ColliderMeshToMesh(Collider* objA, Collider* objB, 
			CollisionContact& collisionContact);

		Collider(void) : colliderObject(0) {}
		~Collider(void);

		ColliderType GetColliderType(void)const;

		const ColliderObject* GetColliderObject(void)const{ return colliderObject; }

		const EDGOCA::AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);
		static void OnCollide( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void OnGetCollider( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* message );
		static void OnAttributeChanged( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );
		static void OnIntersectSegment( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg );

	private:

		ColliderObject* colliderObject;
	};
};