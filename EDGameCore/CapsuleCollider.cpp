#include "CapsuleCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "WheelCollider.h"
#include "MeshCollider.h"
#include "../EDCollision/sphere.h"
#include "../EDCollision/capsule.h"
#include "../EDCollision/obb.h"
#include "../EDCollision/ray.h"

namespace EDGameCore
{
	CapsuleCollider::CapsuleCollider(void)
	{
		capsule.center.makeZero();
		capsule.direction = Float3(0.0f, 0.0f, 1.0f);
		capsule.radius = 1.0f;
		capsule.height = 4.0f;
	}

	void CapsuleCollider::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* centerEle = xmlElement->FirstChildElement("Center");

		if( centerEle != 0 )
		{
			TiXmlElement* xEle = centerEle->FirstChildElement("x");
			if( xEle != 0 &&
				xEle->GetText() != 0 &&
				strlen(xEle->GetText()) != 0 )
			{
				capsule.center.x = (float)atof(xEle->GetText());
			}

			TiXmlElement* yEle = centerEle->FirstChildElement("y");
			if( yEle != 0 &&
				yEle->GetText() != 0 &&
				strlen(yEle->GetText()) != 0 )
			{
				capsule.center.y = (float)atof(yEle->GetText());
			}

			TiXmlElement* zEle = centerEle->FirstChildElement("z");
			if( zEle != 0 &&
				zEle->GetText() != 0 &&
				strlen(zEle->GetText()) != 0 )
			{
				capsule.center.z = (float)atof(zEle->GetText());
			}
		}

		TiXmlElement* directionEle = xmlElement->FirstChildElement("Direction");

		if( directionEle != 0 )
		{
			TiXmlElement* xEle = directionEle->FirstChildElement("x");
			if( xEle != 0 &&
				xEle->GetText() != 0 &&
				strlen(xEle->GetText()) != 0 )
			{
				capsule.direction.x = (float)atof(xEle->GetText());
			}

			TiXmlElement* yEle = directionEle->FirstChildElement("y");
			if( yEle != 0 &&
				yEle->GetText() != 0 &&
				strlen(yEle->GetText()) != 0 )
			{
				capsule.direction.y = (float)atof(yEle->GetText());
			}

			TiXmlElement* zEle = directionEle->FirstChildElement("z");
			if( zEle != 0 &&
				zEle->GetText() != 0 &&
				strlen(zEle->GetText()) != 0 )
			{
				capsule.direction.z = (float)atof(zEle->GetText());
			}
		}

		TiXmlElement* heightEle = xmlElement->FirstChildElement("Height");
		if( heightEle != 0 &&
			heightEle->GetText() != 0 &&
			strlen(heightEle->GetText()) != 0 )
		{
			capsule.height = (float)atof(heightEle->GetText());
		}

		TiXmlElement* radiusEle = xmlElement->FirstChildElement("Radius");
		if( radiusEle != 0 &&
			radiusEle->GetText() != 0 &&
			strlen(radiusEle->GetText()) != 0 )
		{
			capsule.radius = (float)atof(radiusEle->GetText());
		}
	}

	const EDMath::Aabb& CapsuleCollider::GetAabb(void)
	{
		//GameObject* gameObject = GetGameObject();
		//const Float4x4& worldMat = gameObject->GetTransform()->GetWorldMatrix();

		//Float3 dir;

		//TransformPoint( worldAabb.center, capsule.center, worldMat );
		//TransformVector( dir, capsule.direction, worldMat );
		//
		//float offset = capsule.height * 0.5f - capsule.radius;

		//worldAabb.extents = Float3( abs(dir.x), abs(dir.y), abs(dir.z) ) * offset;
		//worldAabb.extents += Float3( capsule.radius, capsule.radius, capsule.radius );

		worldAabb = GetWorldCapsule().GetAabb();

		return worldAabb;
	}

	EDMath::Capsule CapsuleCollider::GetWorldCapsule(void)
	{
		Capsule c;
		c.height = capsule.height;
		c.radius = capsule.radius;

		GameObject* gameObject = GetGameObject();

		TransformPoint( c.center, capsule.center, gameObject->GetTransform()->GetWorldMatrix() );
		TransformVector( c.direction, capsule.direction, gameObject->GetTransform()->GetWorldMatrix() );

		return c;
	}

	bool CapsuleCollider::IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength)
	{
		if( EDCollision::IntersectRayCapsule( start, direction, maxLength, GetWorldCapsule(), rayHit.Point, rayHit.Normal ) )
		{
			rayHit.collider = this;
			rayHit.distance = DotProduct( rayHit.Point - start, direction );
			return true;
		}

		return false;
	}
}