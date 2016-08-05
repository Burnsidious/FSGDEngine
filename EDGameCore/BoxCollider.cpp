#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "../EDCollision/sphere.h"
#include "../EDCollision/capsule.h"
#include "../EDCollision/obb.h"
#include "../EDCollision/ray.h"

namespace EDGameCore
{
	BoxCollider::BoxCollider(void)
	{
		box.center.makeZero();
		box.extents.x = 1.0f;
		box.extents.y = 1.0f;
		box.extents.z = 1.0f;
	}

	void BoxCollider::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* centerEle = xmlElement->FirstChildElement("Center");

		if( centerEle != 0 )
		{
			TiXmlElement* xEle = centerEle->FirstChildElement("x");
			if( xEle != 0 &&
				xEle->GetText() != 0 &&
				strlen(xEle->GetText()) != 0 )
			{
				box.center.x = (float)atof(xEle->GetText());
			}

			TiXmlElement* yEle = centerEle->FirstChildElement("y");
			if( yEle != 0 &&
				yEle->GetText() != 0 &&
				strlen(yEle->GetText()) != 0 )
			{
				box.center.y = (float)atof(yEle->GetText());
			}

			TiXmlElement* zEle = centerEle->FirstChildElement("z");
			if( zEle != 0 &&
				zEle->GetText() != 0 &&
				strlen(zEle->GetText()) != 0 )
			{
				box.center.z = (float)atof(zEle->GetText());
			}
		}

		TiXmlElement* extentsEle = xmlElement->FirstChildElement("Extents");

		if( extentsEle != 0 )
		{
			TiXmlElement* xEle = extentsEle->FirstChildElement("x");
			if( xEle != 0 &&
				xEle->GetText() != 0 &&
				strlen(xEle->GetText()) != 0 )
			{
				box.extents.x = (float)atof(xEle->GetText());
			}

			TiXmlElement* yEle = extentsEle->FirstChildElement("y");
			if( yEle != 0 &&
				yEle->GetText() != 0 &&
				strlen(yEle->GetText()) != 0 )
			{
				box.extents.y = (float)atof(yEle->GetText());
			}

			TiXmlElement* zEle = extentsEle->FirstChildElement("z");
			if( zEle != 0 &&
				zEle->GetText() != 0 &&
				strlen(zEle->GetText()) != 0 )
			{
				box.extents.z = (float)atof(zEle->GetText());
			}
		}
	}

	const EDMath::Aabb& BoxCollider::GetAabb(void)
	{
		//GameObject* gameObject = GetGameObject();

		//const Float4x4& worldMat = gameObject->GetTransform()->GetWorldMatrix();
		//
		//for(int i = 0; i < 3; ++i)
		//{
		//	worldAabb.center.v[i] = worldMat.WAxis.v[i];
		//	worldAabb.extents.v[i] = 0.0f;

		//	for(int j = 0; j < 3; ++j)
		//	{
		//		worldAabb.center.v[i] += worldMat.m[j][i] * box.center.v[j];
		//		worldAabb.extents.v[i] += abs(worldMat.m[j][i]) * box.extents.v[j];
		//	}
		//}

		worldAabb = GetWorldBox().GetAabb();

		return worldAabb;
	}

	EDMath::Obb BoxCollider::GetWorldBox(void)
	{
		Obb b;
		b.extents = box.extents;
		GameObject* gameObject = GetGameObject();

		b.transform = gameObject->GetTransform()->GetWorldMatrix();
		TransformPoint( b.transform.WAxis, box.center, gameObject->GetTransform()->GetWorldMatrix() );

		return b;
	}

	bool BoxCollider::IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength)
	{
		if( EDCollision::IntersectRayObb( start, direction, maxLength, GetWorldBox(), rayHit.Point, rayHit.Normal ) )
		{
			rayHit.collider = this;
			rayHit.distance = DotProduct( rayHit.Point - start, direction );
			return true;
		}

		return false;
	}
}