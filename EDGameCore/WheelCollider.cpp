#include "WheelCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "MeshCollider.h"

namespace EDGameCore
{
	WheelCollider::WheelCollider(void)
	{
		center.makeZero();
		radius = 0.36f;
		suspensionDistance = 0.1f;
		motorTorque = 0.0f;
		brakeTorque = 0.0f;
		wheelSpin = 0.0f;
	}

	Float3 WheelCollider::GetWorldCenter(void)
	{
		Float3 wc;
		TransformPoint( wc, center, GetGameObject()->GetTransform()->GetWorldMatrix() );

		return wc;
	}

	const EDMath::Aabb& WheelCollider::GetAabb(void)
	{
		TransformPoint( worldAabb.center, center, GetGameObject()->GetTransform()->GetWorldMatrix() );
		worldAabb.extents.x = worldAabb.extents.y = worldAabb.extents.z = radius + suspensionDistance;

		return worldAabb;
	}

	Float3 WheelCollider::GetWorldWheelDown(void)
	{
		Transform* transform = GetTransform()->GetParent();

		if( transform == 0 )
			transform = GetTransform();

		Float3 dir = transform->GetWorldMatrix().YAxis;

		return dir.negate();
	}

	Float3 WheelCollider::GetWorldWheelRight(void)
	{
		return GetTransform()->GetWorldMatrix().XAxis;
	}

	Float3 WheelCollider::GetWorldWheelForward(void)
	{
		Float3 down = GetWorldWheelDown();
		Float3 right = GetWorldWheelRight();
		Float3 forward;
		CrossProduct( forward, down, right );

		return forward;
	}

	void WheelCollider::GetWheelMatrix(Float4x4& mat)
	{
		mat.makeIdentity();
		mat.XAxis = GetWorldWheelRight();
		mat.YAxis = GetWorldWheelDown().negate();
		mat.ZAxis = GetWorldWheelForward();
		mat.WAxis = GetWorldCenter();
	}

	void WheelCollider::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* centerEle = xmlElement->FirstChildElement("Center");

		if( centerEle != 0 )
		{
			TiXmlElement* xEle = centerEle->FirstChildElement("x");
			if( xEle != 0 &&
				xEle->GetText() != 0 &&
				strlen(xEle->GetText()) != 0 )
			{
				center.x = (float)atof(xEle->GetText());
			}

			TiXmlElement* yEle = centerEle->FirstChildElement("y");
			if( yEle != 0 &&
				yEle->GetText() != 0 &&
				strlen(yEle->GetText()) != 0 )
			{
				center.y = (float)atof(yEle->GetText());
			}

			TiXmlElement* zEle = centerEle->FirstChildElement("z");
			if( zEle != 0 &&
				zEle->GetText() != 0 &&
				strlen(zEle->GetText()) != 0 )
			{
				center.z = (float)atof(zEle->GetText());
			}
		}

		TiXmlElement* radiusEle = xmlElement->FirstChildElement("Radius");
		
		if( radiusEle != 0 &&
			radiusEle->GetText() != 0 &&
			strlen(radiusEle->GetText()) != 0 )
			radius = (float)atof(radiusEle->GetText());

		TiXmlElement* suspensionEle = xmlElement->FirstChildElement("SuspensionDistance");
		
		if( suspensionEle != 0 &&
			suspensionEle->GetText() != 0 &&
			strlen( suspensionEle->GetText() ) != 0 )
			suspensionDistance = (float)atof(suspensionEle->GetText());
	}

	bool WheelCollider::Collide(ICollider* other, EDCollision::Contact* contact)
	{
		ColliderType otherType = other->GetColliderType();

		switch( otherType )
		{
		case WHEEL:
			{
				return false;
			}
			break;
		case MESH:
			{
				IMeshCollider* meshCollider = (IMeshCollider*)other;
				
				RayHit rayHit;
				if( meshCollider->IntersectRay( GetWorldCenter(), GetWorldWheelDown(), rayHit, GetRadius() ) )
				{
					if( contact )
					{
						contact->Normal = rayHit.Normal;
						contact->Point[0] = rayHit.Point;
						contact->numPoints = 1;
						contact->PenetrationDepth = GetRadius() - rayHit.distance;
					}

					return true;
				}

				return false;
			}
			break;
		default:
			return ICollider::Collide( other, contact );
		}
	}

	bool WheelCollider::Collide(const BoundingVolume& boundingVolume, EDCollision::Contact* contact)
	{
		return false;
	}
}