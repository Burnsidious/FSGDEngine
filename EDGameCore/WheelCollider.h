#pragma once

#include "Collider.h"
#include "../EDMath/aabb.h"

#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"

namespace EDGameCore
{
	class WheelCollider : public ColliderT< WheelCollider >
	{
	public:
		WheelCollider(void);
		
		void SetCenter(const Float3& c){ center = c; }
		void SetRadius(float r){ radius = r; }
		void SetSuspensionDistance(float s){ suspensionDistance = s; }
		
		const Float3& GetCenter(void)const{ return center; }
		float GetRadius(void)const{ return radius; }
		float GetSuspensionDistance(void)const{ return suspensionDistance; }

		Float3 GetWorldCenter(void);
		Float3 GetWorldWheelDown(void);
		Float3 GetWorldWheelRight(void);
		Float3 GetWorldWheelForward(void);
		void GetWheelMatrix(Float4x4& mat);

		const EDMath::Aabb& GetAabb(void);
		ColliderType GetColliderType(void)const{ return WHEEL; }

		float GetMotorTorque(void)const{ return motorTorque; }
		float GetBrakeTorque(void)const{ return brakeTorque; }

		void SetMotorTorque(float t){ motorTorque = t; }
		void SetBrakeTorque(float t){ brakeTorque = t; }

		float GetWheelSpin(void){ return wheelSpin; }
		void SetWheelSpin(float w){ wheelSpin = w; }

		bool Collide(ICollider* other, EDCollision::Contact* contact = 0);
		bool Collide(const BoundingVolume& boundingVolume, EDCollision::Contact* contact = 0);

	private:
		Float3 center;
		float radius;
		float suspensionDistance;
		float motorTorque;
		float brakeTorque;
		float wheelSpin;

		void LoadState(TiXmlElement* xmlElement);
	};
}