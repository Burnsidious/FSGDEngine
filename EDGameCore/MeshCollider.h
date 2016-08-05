#pragma once

#include "Collider.h"
#include "../EDMath/aabb.h"
#include "../EDCollision/EDBVH.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "WheelCollider.h"

namespace EDGameCore
{
	class IMeshCollider : public ICollider
	{
	public:
		virtual ~IMeshCollider(){}
		
		//virtual bool Collide(const Sphere& sphere, EDCollision::Contact& contact) = 0;
		//virtual bool Collide(const Capsule& capsule, EDCollision::Contact& contact) = 0;
		//virtual bool Collide(const Obb& obb, EDCollision::Contact& contact) = 0;
		//virtual bool OverlapRay(Float3 start, Float3 direction, float maxLength) = 0;
		//virtual bool OverlapSphere(const Sphere& sphere) = 0;
		//virtual bool IntersectRay(Float3 start, Float3 direction, float maxLength, RayHit& rayHit) = 0;
		//virtual bool IntersectSphere(const Sphere& sphere, EDCollision::Contact& contact) = 0;

		virtual const EDMath::WingedTriangle* GetTriangle(unsigned int triIndex) const = 0;
		virtual EDMath::Float3 GetTriNormal(unsigned int triIndex) const = 0;
		virtual EDMath::Float3 GetPoint(unsigned int vertexIndex) const = 0;
		void GetPoints(unsigned int triIndex, EDMath::Float3& a, EDMath::Float3& b, EDMath::Float3& c) const;
		void GetPoints(const EDMath::WingedTriangle& triangle, EDMath::Float3& a, EDMath::Float3& b, EDMath::Float3& c) const;

		ColliderType GetColliderType(void)const{ return MESH; }
	};

	template<typename Derived>
	class MeshCollider : public IMeshCollider
	{
		friend class Game;
		friend class Registry;

	public:
		virtual ~MeshCollider(void){}

		
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
	RegistrationId MeshCollider<Derived>::typeId = 0;
	
	template<typename Derived>
	string MeshCollider<Derived>::typeName;

	class BVHCollider : public MeshCollider<BVHCollider>
	{
	public:
		BVHCollider(void) : bvhPtr(0) {}
		~BVHCollider();

		const EDMath::Aabb& GetAabb(void);

		void LoadState(TiXmlElement* xmlElement);

		bool Collide(ICollider* other, EDCollision::Contact* contact = 0);
		bool Collide(const BoundingVolume& boundingVolume, EDCollision::Contact* contact = 0);

		bool OverlapRay(Float3 start, Float3 direction, float maxLength = FLT_MAX);
		bool IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength = FLT_MAX);

		const EDMath::WingedTriangle* GetTriangle(unsigned int triIndex) const;
		EDMath::Float3 GetTriNormal(unsigned int triIndex) const;
		EDMath::Float3 GetPoint(unsigned int vertexIndex) const;

	private:
		EDCollision::EDBVH* bvhPtr;

		ContentHandle<EDCollision::EDBVH> bvhHandle;
	};
}