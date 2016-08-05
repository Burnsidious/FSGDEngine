#pragma once

#include "Collider.h"
#include "../EDMath/aabb.h"
#include "../EDMath/capsule.h"

namespace EDGameCore
{
	class CapsuleCollider : public ColliderT< CapsuleCollider >
	{
	public:
		CapsuleCollider(void);
		void LoadState(TiXmlElement* xmlElement);
		
		void SetCapsule(const EDMath::Capsule& c){ capsule = c; }
		const EDMath::Capsule& GetCapsule(void){ return capsule; }
		EDMath::Capsule GetWorldCapsule(void);

		const EDMath::Aabb& GetAabb(void);
		ColliderType GetColliderType(void)const{ return CAPSULE; }

		bool IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength = FLT_MAX);
	private:
		EDMath::Capsule capsule;
	};
}