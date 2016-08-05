#include "EDCollision.h"
#include "sphere.h"
#include "capsule.h"
#include "aabb.h"
#include "obb.h"
#include "triangle.h"

namespace EDCollision
{
	void Contact::negate(void)
	{
		for(unsigned int i = 0; i < numPoints; ++i)
			Point[i] -= Normal * PenetrationDepth;

		Normal.negate();
	}

	int PointOutsideOfPlane(Float3 p, Float3 a, Float3 b, Float3 c, Float3 d)
	{
		Float3 abc;
		CrossProduct( abc, b - a, c - a );
		float signp = DotProduct( p - a, abc );
		float signd = DotProduct( d - a, abc );
		return signp * signd < 0.0f;
	}

	float ClosestPtPointTetrahedron(Float3 p, Float3 a, Float3 b, Float3 c, Float3 d, Float3& closestPt)
	{
		closestPt = p;
		float bestSqDist = FLT_MAX;

		if( PointOutsideOfPlane( p, a, b, c, d ) )
		{
			Float3 q;
			
			float sqDist = ClosestPtTriangle( p, a, b, c, q );

			if( sqDist < bestSqDist )
			{
				bestSqDist = sqDist;
				closestPt = q;
			}
		}

		if( PointOutsideOfPlane( p, a, c, d, b ) )
		{
			Float3 q;
	
			float sqDist = ClosestPtTriangle( p, a, c, d, q );

			if( sqDist < bestSqDist )
			{
				bestSqDist = sqDist;
				closestPt = q;
			}
		}

		if( PointOutsideOfPlane( p, a, d, b, c ) )
		{
			Float3 q;
			
			float sqDist = ClosestPtTriangle( p, a, d, b, q );

			if( sqDist < bestSqDist )
			{
				bestSqDist = sqDist;
				closestPt = q;
			}
		}

		if( PointOutsideOfPlane( p, b, d, c, a ) )
		{
			Float3 q;
						
			float sqDist = ClosestPtTriangle( p, b, d, c, q );

			if( sqDist < bestSqDist )
			{
				bestSqDist = sqDist;
				closestPt = q;
			}
		}

		return bestSqDist != FLT_MAX ? bestSqDist : 0.0f;
	}

	void Contact::getClosestPoint(const Float3& pt, Float3& c)const
	{
		if( numPoints == 0 )
			return;

		#if 1

		//float d = (Point[0] - pt).squaredMagnitude();
		//int index = 0;

		//for(int i = 1; i < numPoints; ++i)
		//{
		//	float d2 = (Point[i] - pt).squaredMagnitude();
		//	if( d2 < d )
		//	{
		//		d = d2;
		//		index = i;
		//	}
		//}

		//c = Point[index];

		c.makeZero();
		for(unsigned int i = 0; i < numPoints; ++i)
			c += Point[i];
		
		c *= (1.0f / numPoints);
		#endif

		#if 0
		switch( min(3, numPoints) )
		{
		case 3:
			{
				Float3 perp;
				CrossProduct( perp, Point[1] - Point[0], Point[2] - Point[1] );

				float perpMagSq = DotProduct( perp, perp );
				if( perpMagSq > FLT_EPSILON )
				{
					float ratio = -DotProduct( pt - Point[0], perp ) / perpMagSq;
					
					Float3 projPt = pt + perp*ratio;

					Float3 startV = Point[ numPoints-1 ];
					size_t i = 0;
					for(; i < numPoints; ++i)
					{
						Float3 endV = Point[i];

						Float3 edge = endV - startV;
						Float3 edgeN;
						CrossProduct( edgeN, edge, perp );

						Float3 v = projPt - startV;
						if( DotProduct( v, edgeN ) > 0.0f )
						{
							float ratio = max(0.0f, min( 1.0f, DotProduct( v, edge ) / DotProduct( edge, edge ) ) );
							c = startV + edge * ratio;
							break;
						}

						startV = endV;
					}

					if( i == numPoints )
						c = projPt;

					break;
				}
			}
		case 2:
			{
				Float3 d = Point[1] - Point[0];
				float edgeLenSq = DotProduct( d, d );
						
				if( edgeLenSq > FLT_EPSILON )
				{
					Float3 v = pt - Point[0];

					float ratio = max( 0.0f, min( 1.0f, DotProduct(v, d) / edgeLenSq ) );

					c = Point[0] + d * ratio;

					break;
				}
			}
		default:
			{
				c = Point[0];
			}
			break;
		}
	#endif
	}

	bool Collide(const EDMath::BoundingVolume* first, const EDMath::BoundingVolume* second, Contact* contact)
	{
		bool result = false;
		bool flip = false;

		EDMath::BoundingVolume::Type firstType = first->GetType();
		EDMath::BoundingVolume::Type secondType = second->GetType();

		if(	first->GetType() > second->GetType() )
		{
			flip = true;
			std::swap( first, second );
			std::swap( firstType, secondType );
		}

		switch( firstType )
		{
		case EDMath::BoundingVolume::SPHERE :
			{
				switch( secondType )
				{
					case EDMath::BoundingVolume::SPHERE :
						{
							result = SphereToSphere( *(Sphere*)first, *(Sphere*)second, contact );
						}
						break;
					case EDMath::BoundingVolume::CAPSULE :
						{
							result = SphereToCapsule( *(Sphere*)first, *(Capsule*)second, contact );
						}
						break;
					case EDMath::BoundingVolume::AABB :
						{
							result = SphereToAabb( *(Sphere*)first, *(Aabb*)second, contact );
						}
						break;
					case EDMath::BoundingVolume::OBB :
						{
							result = SphereToObb( *(Sphere*)first, *(Obb*)second, contact );
						}
						break;
					default:
						break;
				}
			}
			break;
		case EDMath::BoundingVolume::CAPSULE :
			{
				switch( secondType )
				{
					case EDMath::BoundingVolume::CAPSULE :
						{
							result = CapsuleToCapsule( *(Capsule*)first, *(Capsule*)second, contact );
						}
						break;
					case EDMath::BoundingVolume::AABB :
						{
							result = CapsuleToAabb( *(Capsule*)first, *(Aabb*)second, contact );
						}
						break;
					case EDMath::BoundingVolume::OBB :
						{
							result = CapsuleToObb( *(Capsule*)first, *(Obb*)second, contact );
						}
						break;
					default:
						break;
				}
			}
			break;
		case EDMath::BoundingVolume::AABB :
			{
				switch( secondType )
				{
					case EDMath::BoundingVolume::AABB :
						{
							result = AabbToAabb( *(Aabb*)first, *(Aabb*)second, contact );
						}
						break;
					case EDMath::BoundingVolume::OBB :
						{
							result = AabbToObb( *(Aabb*)first, *(Obb*)second, contact );
						}
						break;
					default:
						break;
				}			
			}
			break;
		case EDMath::BoundingVolume::OBB :
			{
				switch( secondType )
				{
					case EDMath::BoundingVolume::OBB :
						{
							result = ObbToObb( *(Obb*)first, *(Obb*)second, contact );
						}
						break;
					default:
						break;
				}	
			}
			break;
		default:
			break;
		}

		if( contact && result && flip )
			contact->negate();

		return result;
	}

	bool EDCollision::CollideTriangle(const EDMath::BoundingVolume& boundingVolume, const Float3& a, const Float3& b, const Float3& c, Contact* contact)
	{
		switch( boundingVolume.GetType() )
		{
		case BoundingVolume::SPHERE :
			{
				return EDCollision::SphereToTriangle( (const Sphere&)boundingVolume, a, b, c, contact );
			}
			break;
		case BoundingVolume::CAPSULE :
			{
				return EDCollision::CapsuleToTriangle( (const Capsule&)boundingVolume, a, b, c, contact );
			}
			break;
		case BoundingVolume::AABB :
			{
				Obb obb;
				obb.transform.makeIdentity();
				obb.transform.WAxis = ((const Aabb&)boundingVolume).center;
				obb.extents = ((const Aabb&)boundingVolume).extents;

				return EDCollision::ObbToTriangle( obb, a, b, c, contact );
			}
			break;
		case BoundingVolume::OBB :
			{
				return EDCollision::ObbToTriangle( (const Obb&)boundingVolume, a, b, c, contact );
			}
			break;
		default:
			break;
		}

		return false;
	}
}