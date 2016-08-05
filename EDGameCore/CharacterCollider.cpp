#include "CharacterCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "../EDCollision/ray.h"

namespace EDGameCore
{
	CharacterCollider::CharacterCollider(void)
	{
		capsule.center = Float3(0.0f, 1.125f, 0.0f);
		capsule.direction = Float3(0.0f, -1.0f, 0.0f);
		capsule.radius = 0.25f;
		capsule.height = 1.75f;
		standY = 0.866f;
		stopY = 0.7071f;
	}

	void CharacterCollider::LoadState(TiXmlElement* xmlElement)
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

		TiXmlElement* standEle = xmlElement->FirstChildElement("StandLimit");
		if( standEle != 0 &&
			standEle->GetText() != 0 &&
			strlen(standEle->GetText()) != 0 )
		{
			float standLimit = (float)atof(standEle->GetText());

			SetStandLimit( standLimit );
		}

		TiXmlElement* stopEle = xmlElement->FirstChildElement("StopLimit");
		if( stopEle != 0 &&
			stopEle->GetText() != 0 &&
			strlen(stopEle->GetText()) != 0 )
		{
			float stopLimit = (float)atof(stopEle->GetText());
			
			SetStopLimit( stopLimit );
		}
	}

	void CharacterCollider::SetStandLimit(float standLimit)
	{
		standLimit = max( 0.0f, min( 90.0f, standLimit ) );

		standY = sinf( standLimit * 3.14159f / 180.0f );			
	}

	void CharacterCollider::SetStopLimit(float stopLimit)
	{
		stopLimit = max(0.0f, min( 90.0f, stopLimit ) );

		stopY = sinf( stopLimit * 3.14159f / 180.0f );

		if( standY < stopY )
			stopY = standY;
	}

	const EDMath::Aabb& CharacterCollider::GetAabb(void)
	{
		worldAabb = GetWorldCapsule().GetAabb();

		return worldAabb;
	}

	EDMath::Capsule CharacterCollider::GetWorldCapsule(void)
	{
		Capsule c;
		c.height = capsule.height;
		c.radius = capsule.radius;

		GameObject* gameObject = GetGameObject();

		TransformPoint( c.center, capsule.center, gameObject->GetTransform()->GetWorldMatrix() );
		TransformVector( c.direction, capsule.direction, gameObject->GetTransform()->GetWorldMatrix() );

		return c;
	}

	bool CharacterCollider::IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength)
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