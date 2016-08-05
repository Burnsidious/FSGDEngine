#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "MeshCollider.h"
#include "WheelCollider.h"

#include "../EDCollision/sphere.h"
#include "../EDCollision/ray.h"

namespace EDGameCore
{
	SphereCollider::SphereCollider(void)
	{
		sphere.center.makeZero();
		sphere.radius = 1.0f;
	}

	void SphereCollider::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* centerEle = xmlElement->FirstChildElement("Center");

		if( centerEle != 0 )
		{
			TiXmlElement* xEle = centerEle->FirstChildElement("x");
			if( xEle != 0 &&
				xEle->GetText() != 0 &&
				strlen(xEle->GetText()) != 0 )
			{
				sphere.center.x = (float)atof(xEle->GetText());
			}

			TiXmlElement* yEle = centerEle->FirstChildElement("y");
			if( yEle != 0 &&
				yEle->GetText() != 0 &&
				strlen(yEle->GetText()) != 0 )
			{
				sphere.center.y = (float)atof(yEle->GetText());
			}

			TiXmlElement* zEle = centerEle->FirstChildElement("z");
			if( zEle != 0 &&
				zEle->GetText() != 0 &&
				strlen(zEle->GetText()) != 0 )
			{
				sphere.center.z = (float)atof(zEle->GetText());
			}
		}

		TiXmlElement* radiusEle = xmlElement->FirstChildElement("Radius");
		
		if( radiusEle != 0 &&
			radiusEle->GetText() != 0 &&
			strlen(radiusEle->GetText()) != 0 )
			sphere.radius = (float)atof(radiusEle->GetText());
	}

	const EDMath::Aabb& SphereCollider::GetAabb(void)
	{
		//GameObject* gameObject = GetGameObject();
		//const Float4x4& worldMat = gameObject->GetTransform()->GetWorldMatrix();

		//TransformPoint( worldAabb.center, sphere.center, worldMat );
		//worldAabb.extents.x = sphere.radius;
		//worldAabb.extents.y = sphere.radius;
		//worldAabb.extents.z = sphere.radius;

		worldAabb = GetWorldSphere().GetAabb();

		return worldAabb;
	}
	
	EDMath::Sphere SphereCollider::GetWorldSphere(void)
	{
		Sphere s;
		s.radius = sphere.radius;
		TransformPoint( s.center, sphere.center, GetGameObject()->GetTransform()->GetWorldMatrix() );
		return s;
	}

	bool SphereCollider::IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength)
	{
		if( EDCollision::IntersectRaySphere( start, direction, maxLength, GetWorldSphere(), rayHit.Point, rayHit.Normal ) )
		{
			rayHit.collider = this;
			rayHit.distance = DotProduct( rayHit.Point - start, direction );
			return true;
		}

		return false;
	}
}