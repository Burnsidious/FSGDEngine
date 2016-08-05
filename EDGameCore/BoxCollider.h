#pragma once

#include "Collider.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"

namespace EDGameCore
{
	class BoxCollider : public ColliderT< BoxCollider >
	{
	public:
		BoxCollider(void);
		void LoadState(TiXmlElement* xmlElement);
		
		void SetBox(const EDMath::Aabb& b){ box = b; }
		const EDMath::Aabb& GetBox(void){ return box; }
		EDMath::Obb GetWorldBox(void);

		const EDMath::Aabb& GetAabb(void);
		ColliderType GetColliderType(void)const{ return BOX; }
		bool IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength = FLT_MAX);

	private:
		EDMath::Aabb box;
	};
}