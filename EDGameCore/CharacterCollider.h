#pragma once

#include "Collider.h"
#include "../EDMath/aabb.h"
#include "../EDMath/capsule.h"

namespace EDGameCore
{
	class CharacterCollider : public ColliderT< CharacterCollider >
	{
	public:
		CharacterCollider(void);
		void LoadState(TiXmlElement* xmlElement);
		
		void SetCapsule(const EDMath::Capsule& c){ capsule = c; }
		const EDMath::Capsule& GetCapsule(void){ return capsule; }
		EDMath::Capsule GetWorldCapsule(void);

		void SetStandLimit(float s);
		void SetStopLimit(float s);

		float GetStandY(void){ return standY; }
		float GetStopY(void){ return stopY; }

		const EDMath::Aabb& GetAabb(void);
		ColliderType GetColliderType(void)const{ return CHARACTER; }

		bool IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength = FLT_MAX);
	private:
		EDMath::Capsule capsule;
		float standY;
		float stopY;
	};
}