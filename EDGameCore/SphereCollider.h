#pragma once

#include "Collider.h"
#include "../EDMath/aabb.h"
#include "../EDMath/sphere.h"


namespace EDGameCore
{
	class SphereCollider : public ColliderT< SphereCollider >
	{
	public:
		SphereCollider(void);
		
		void SetSphere(const EDMath::Sphere& s){ sphere = s; }
		const EDMath::Sphere& GetSphere(void){ return sphere; }
		EDMath::Sphere GetWorldSphere(void);

		const EDMath::Aabb& GetAabb(void);
		ColliderType GetColliderType(void)const{ return SPHERE; }

		bool IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength = FLT_MAX);
	private:
		EDMath::Sphere sphere;

		void LoadState(TiXmlElement* xmlElement);
	};
}