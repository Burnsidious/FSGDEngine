#include "MeshCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "../EDCollision/sphere.h"
#include "../EDCollision/capsule.h"
#include "../EDCollision/obb.h"
#include "../EDCollision/ray.h"
#include "../EDMath/polygon.h"


namespace EDGameCore
{
	void IMeshCollider::GetPoints(unsigned int triIndex, EDMath::Float3& a, EDMath::Float3& b, EDMath::Float3& c) const
	{
		const EDMath::WingedTriangle* triangle = GetTriangle(triIndex);
		
		if( triangle != 0 )
			GetPoints( *triangle, a, b, c );
	}

	void IMeshCollider::GetPoints(const EDMath::WingedTriangle& triangle, EDMath::Float3& a, EDMath::Float3& b, EDMath::Float3& c) const
	{
		a = GetPoint( triangle.i0 );
		b = GetPoint( triangle.i1 );
		c = GetPoint( triangle.i2 );
	}

	BVHCollider::~BVHCollider()
	{
	}

	void BVHCollider::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

		if( pathEle != 0 &&
			pathEle->GetText() != 0 &&
			strlen( pathEle->GetText() ) != 0 )
		{
			bvhHandle = ContentManager::Load<EDCollision::EDBVH>( pathEle->GetText() );
			bvhPtr = bvhHandle.GetContent();
		}
	}

	void ClipTriangleAabb(const Float3& center, const Float3& extents, const Float3& a, const Float3& b, const Float3& c, Float3* newVerts, unsigned int& newVertCount)
	{
		Float3 inV[ EDCollision::Contact::CONTACT_MAX ] = { a, b, c };
		unsigned int inVCount = 3;

		Float3 offs[2] = { center + extents, center + extents };
		float ns[2] = { 1.0f, -1.0f };

		for(int A = 0; A < 3; ++A)
		{
			for(int P = 0; P < 2; ++P)
			{
				Float3 outV[EDCollision::Contact::CONTACT_MAX ];
				unsigned int outVCount = 0;

				Float3 N(0.0f, 0.0f, 0.0f); 
				N.v[A] = ns[P];

				clipPolygonToPlaneNegative( N, offs[P].v[A], inV, inVCount, outV, outVCount );
				memcpy_s( inV, sizeof(inV), outV, sizeof(Float3)*outVCount );
				inVCount = outVCount;
			}
		}

		memcpy_s( newVerts, sizeof(Float3) * EDCollision::Contact::CONTACT_MAX, inV, sizeof(Float3) *inVCount );
		newVertCount = inVCount;
	}

	void ClipTriangleObb(const Obb& obb, const Float3& a, const Float3& b, const Float3& c, Float3* newVerts, unsigned int& newVertCount)
	{
		Float3 v[3];

		InvTransformPoint( v[0], a, obb.transform );
		InvTransformPoint( v[1], b, obb.transform );
		InvTransformPoint( v[2], c, obb.transform );

		ClipTriangleAabb( Float3(0.0f, 0.0f, 0.0f), obb.extents, v[0], v[1], v[2], newVerts, newVertCount);

		for(unsigned int i = 0; i < newVertCount; ++i)
			TransformPoint( newVerts[i], newVerts[i], obb.transform );
	}

	void RayToBVH(Float3 start, Float3 direction, const EDCollision::EDBVH::Node* current, const EDCollision::EDBVH* bvh, RayHit& rayHit)
	{
		if( EDCollision::OverlapRayAabb(start, direction, rayHit.distance, current->getBounds() ) )
		{
			//if( !current->isLeaf() )
			if( current->getNodeType() == EDCollision::EDBVH::Node::BRANCH )
			{
				//const EDCollision::EDBVH::BranchNode* branch = (const EDCollision::EDBVH::BranchNode*)current;
				
				RayToBVH( start, direction, current->getLeftChild(), bvh, rayHit );
				RayToBVH( start, direction, current->getRightChild(), bvh, rayHit );
			}
			else
			{
				//const EDCollision::EDBVH::LeafNode* leaf = (const EDCollision::EDBVH::LeafNode*)current;
				unsigned int startIndex = (unsigned int)current->getStartIndex();
				unsigned int endIndex = (unsigned int)current->getStartIndex() + (unsigned int)current->getTriCount();

				const EDMath::WingedTriangle* tris = bvh->getTriangles();
				const Float3* verts = bvh->getVertices();

				EDCollision::Contact newContact;

				for(unsigned int index = startIndex; index != endIndex; ++index)
				{
					const EDMath::WingedTriangle& tri = tris[index];
					const Float3& norm = bvh->getTriangleNormals()[index];

					if( DotProduct( start - verts[ tri.i0 ], norm ) > 0.0f )
					{
						unsigned int indices[3] = { tri.i0, tri.i1, tri.i2 };

						if( indices[1] < indices[0] )
							swap( indices[1], indices[0] );
						if( indices[2] < indices[1] )
							swap( indices[2], indices[1] );
						if( indices[1] < indices[0] )
							swap( indices[1], indices[0] );

						float t;
						Float3 bc;
						if( EDCollision::IntersectRayTriangle( start, direction, rayHit.distance, verts[indices[0]], verts[indices[1]], verts[indices[2]], bc, t ) )
						{
							rayHit.distance = t;
							rayHit.Normal = norm;
							rayHit.triIndex = index;
							rayHit.Barycentric = bc;
							rayHit.Point = Float3( verts[indices[0]] * bc.x + verts[indices[1]] * bc.y + verts[indices[2]] * bc.z );
						}
					}
				}
			}
		}
	}

	const EDMath::WingedTriangle* BVHCollider::GetTriangle(unsigned int triIndex) const
	{
		if( triIndex < bvhPtr->getTriangleCount() )
			return &bvhPtr->getTriangles()[triIndex];
		
		return 0;
	}

	EDMath::Float3 BVHCollider::GetTriNormal(unsigned int triIndex) const
	{
		Float3 normal(0.0f, 0.0f, 0.0f);

		if( triIndex < bvhPtr->getTriangleCount() )
			normal = bvhPtr->getTriangleNormals()[triIndex];

		return normal;
	}

	EDMath::Float3 BVHCollider::GetPoint(unsigned int vertexIndex) const
	{
		Float3 point(0.0f, 0.0f, 0.0f);

		if( vertexIndex < bvhPtr->getVertexCount() )
			point = bvhPtr->getVertices()[vertexIndex];

		return point;		
	}

	bool BVHCollider::OverlapRay(Float3 start, Float3 direction, float maxLength)
	{
		if( bvhPtr == 0 )
			return false;

		Float3 localStart;
		Float3 localDirection;
		RayHit result;
		result.distance = maxLength;

		InvTransformPoint( localStart, start, GetGameObject()->GetTransform()->GetWorldMatrix() );
		InvTransformVector( localDirection, direction, GetGameObject()->GetTransform()->GetWorldMatrix() );
		
		RayToBVH( localStart, localDirection, bvhPtr->getRoot(), bvhPtr, result );

		return result.distance != maxLength;
	}

	////bool BVHCollider::OverlapSphere(const Sphere& sphere)
	////{
	////	if( bvhPtr == 0 )
	////		return false;

	////	Sphere localSphere;
	////	localSphere.radius = sphere.radius;
	////	InvTransformPoint( localSphere.center, sphere.center, GetGameObject()->GetTransform()->GetWorldMatrix() );

	////	EDCollision::Contact result;
	////	result.numPoints = 0;
	////	result.PenetrationDepth = FLT_MAX;

	////	SphereToBVH( localSphere, bvhPtr->getRoot(), bvhPtr, result );

	////	return result.numPoints != 0;
	////}

	bool BVHCollider::IntersectRay(Float3 start, Float3 direction, RayHit& rayHit, float maxLength )
	{
		if( bvhPtr == 0 )
			return false;

		Float3 localStart;
		Float3 localDirection;
		RayHit result;
		result.distance = maxLength;

		InvTransformPoint( localStart, start, GetGameObject()->GetTransform()->GetWorldMatrix() );
		InvTransformVector( localDirection, direction, GetGameObject()->GetTransform()->GetWorldMatrix() );
		
		RayToBVH( localStart, localDirection, bvhPtr->getRoot(), bvhPtr, result );

		if( result.distance != maxLength )
		{
			TransformPoint( rayHit.Point, result.Point, GetGameObject()->GetTransform()->GetWorldMatrix() );
			TransformVector( rayHit.Normal, result.Normal, GetGameObject()->GetTransform()->GetWorldMatrix() );

			rayHit.distance = result.distance;
			rayHit.triIndex = result.triIndex;
			rayHit.Barycentric = result.Barycentric;

			return true;
		}

		return false;
	}

	////bool BVHCollider::IntersectSphere(const Sphere& sphere, EDCollision::Contact& contact)
	////{
	////	if( bvhPtr == 0 )
	////		return false;

	////	Sphere localSphere;
	////	InvTransformPoint( localSphere.center, sphere.center, GetGameObject()->GetTransform()->GetWorldMatrix() );
	////	localSphere.radius = sphere.radius;

	////	EDCollision::Contact localContact;
	////	localContact.numPoints = 0;

	////	SphereToBVH( localSphere, bvhPtr->getRoot(), bvhPtr, localContact );

	////	if( localContact.numPoints != 0 )
	////	{
	////		TransformPoint( contact.Point[0], localContact.Point[0], GetGameObject()->GetTransform()->GetWorldMatrix() );
	////		TransformVector( contact.Normal, localContact.Normal, GetGameObject()->GetTransform()->GetWorldMatrix() );
	////		contact.numPoints = 1;
	////		contact.PenetrationDepth = localContact.PenetrationDepth;

	////		return true;
	////	}

	////	return false;
	////}

	//bool BVHCollider::Collide(SphereCollider& sphereCollider)
	//{
	//	if( bvhPtr == 0 )
	//		return false;

	//	Sphere worldSphere = sphereCollider.GetWorldSphere();
	//	Sphere localSphere;
	//	localSphere.radius = worldSphere.radius;
	//	InvTransformPoint( localSphere.center, worldSphere.center, GetGameObject()->GetTransform()->GetWorldMatrix() );

	//	EDCollision::Contact result;
	//	result.numPoints = 0;
	//	result.PenetrationDepth = -FLT_MAX;

	//	SphereToBVH( localSphere, bvhPtr->getRoot(), bvhPtr, result );

	//	return result.PenetrationDepth > 0.01f;
	//}

	//bool BVHCollider::Collide(CapsuleCollider& capsuleCollider)
	//{
	//	return false;
	//}

	//bool BVHCollider::Collide(BoxCollider& boxCollider)
	//{
	//	if( bvhPtr == 0 )
	//		return false;

	//	EDCollision::Contact localContact;
	//	localContact.PenetrationDepth = -FLT_MAX;
	//	
	//	Obb worldObb = boxCollider.GetWorldBox();
	//	Obb localObb = worldObb;
	//	
	//	const Float4x4& bvhWorld = GetGameObject()->GetTransform()->GetWorldMatrix();
	//	InvTransformObb( localObb, worldObb, bvhWorld );
	//	
	//	unsigned int triIndex = 0xFFFFFFFF;
	//	ObbToBVH( localObb, bvhPtr->getRoot(), bvhPtr, localContact, triIndex );

	//	return localContact.PenetrationDepth > 0.01f;
	//}

	//bool BVHCollider::Collide(WheelCollider& wheelCollider)
	//{
	//	return false;
	//}

	//bool BVHCollider::Collide(IMeshCollider& meshCollider)
	//{
	//	return false;
	//}

	//bool BVHCollider::Intersect(SphereCollider& sphereCollider, EDCollision::Contact& contact)
	//{
	//	if( bvhPtr == 0 )
	//		return false;

	//	Sphere worldSphere = sphereCollider.GetWorldSphere();
	//	Sphere localSphere;
	//	localSphere.radius = worldSphere.radius;
	//	InvTransformPoint( localSphere.center, worldSphere.center, GetGameObject()->GetTransform()->GetWorldMatrix() );

	//	EDCollision::Contact result;
	//	result.numPoints = 0;
	//	result.PenetrationDepth = -FLT_MAX;

	//	SphereToBVH( localSphere, bvhPtr->getRoot(), bvhPtr, result );

	//	if( result.PenetrationDepth > 0.01f )
	//	{
	//		TransformPoint( contact.Point[0], result.Point[0], GetGameObject()->GetTransform()->GetWorldMatrix() );
	//		TransformVector( contact.Normal, result.Normal, GetGameObject()->GetTransform()->GetWorldMatrix() );
	//		contact.PenetrationDepth = result.PenetrationDepth - 0.01f;
	//		contact.numPoints = result.numPoints;

	//		return true;
	//	}

	//	return false;
	//}

	//bool BVHCollider::Intersect(CapsuleCollider& capsuleCollider, EDCollision::Contact& contact)
	//{
	//	return false;
	//}

	//bool BVHCollider::Intersect(BoxCollider& boxCollider, EDCollision::Contact& contact)
	//{
	//	if( bvhPtr == 0 )
	//		return false;

	//	EDCollision::Contact localContact;
	//	localContact.PenetrationDepth = -FLT_MAX;
	//	
	//	Obb worldObb = boxCollider.GetWorldBox();
	//	Obb localObb = worldObb;
	//	
	//	const Float4x4& bvhWorld = GetGameObject()->GetTransform()->GetWorldMatrix();
	//	InvTransformObb( localObb, worldObb, bvhWorld );
	//	
	//	unsigned int triIndex = 0xFFFFFFFF;
	//	ObbToBVH( localObb, bvhPtr->getRoot(), bvhPtr, localContact, triIndex );

	//	if( localContact.PenetrationDepth > 0.01f )
	//	{
	//		const EDMath::WingedTriangle* tris = bvhPtr->getTriangles();
	//		const Float3* verts = bvhPtr->getVertices();
	//		const EDMath::WingedTriangle& tri = tris[triIndex];

	//		TransformVector( contact.Normal, localContact.Normal, bvhWorld );
	//		contact.PenetrationDepth = localContact.PenetrationDepth;

	//		Float3 cd = (contact.Normal * contact.PenetrationDepth * 0.95f);

	//		Float3 v[3];
	//		for(int i = 0; i < 3; ++i)
	//		{
	//			TransformPoint( v[i], verts[ tri.indices[i] ], bvhWorld );

	//			v[i] -= cd;
	//		}


	//		ClipTriangleObb( worldObb, v[0], v[1], v[2], contact.Point, contact.numPoints);

	//		return contact.numPoints != 0;
	//	}

	//	return false;
	//}

	//bool BVHCollider::Intersect(WheelCollider& wheelCollider, EDCollision::Contact& contact)
	//{
	//	return false;
	//}

	//bool BVHCollider::Intersect(IMeshCollider& meshCollider, EDCollision::Contact& contact)
	//{
	//	return false;
	//}

	bool BVHCollider::Collide(ICollider* other, EDCollision::Contact* contact)
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
				return false;
			}
			break;
		default:
			return ICollider::Collide( other, contact );
		}		
	}

	bool BVHCollider::Collide(const BoundingVolume& boundingVolume, EDCollision::Contact* contact)
	{
		switch( boundingVolume.GetType() )
		{
		case EDMath::BoundingVolume::SPHERE:
			{
				Sphere localSphere = (const Sphere&)boundingVolume;
				EDMath::InvTransformPoint( localSphere.center, localSphere.center, GetTransform()->GetWorldMatrix() );

				EDCollision::Contact localContact;
				localContact.PenetrationDepth = -FLT_MAX;
				unsigned int triIndex = 0xFFFFFFFF;

				
				//SphereToBVH( localSphere, bvhPtr->getRoot(), bvhPtr, localContact );

				//if( localContact.PenetrationDepth > 0.0f )
				if( bvhPtr->Collide( localSphere, triIndex, &localContact ) )
				{
					if( contact )
					{
						contact->numPoints = localContact.numPoints;
						contact->PenetrationDepth = localContact.PenetrationDepth;
						EDMath::TransformPoint( contact->Point[0], localContact.Point[0], GetTransform()->GetWorldMatrix() );
						EDMath::TransformVector( contact->Normal, localContact.Normal, GetTransform()->GetWorldMatrix() );
					}

					return true;
				}
			}
			break;
		case EDMath::BoundingVolume::CAPSULE:
			{
				Capsule localCapsule = (const Capsule&)boundingVolume;
				EDMath::InvTransformPoint( localCapsule.center, localCapsule.center, GetTransform()->GetWorldMatrix() );
				EDMath::InvTransformVector( localCapsule.direction, localCapsule.direction, GetTransform()->GetWorldMatrix() );

				EDCollision::Contact localContact;
				localContact.PenetrationDepth = -FLT_MAX;
				unsigned int triIndex = 0xFFFFFFFF;

				if( bvhPtr->Collide( localCapsule, triIndex, &localContact ) )
				{
					if( contact )
					{
						contact->numPoints = localContact.numPoints;
						contact->PenetrationDepth = localContact.PenetrationDepth;
						EDMath::TransformPoint( contact->Point[0], localContact.Point[0], GetTransform()->GetWorldMatrix() );
						EDMath::TransformVector( contact->Normal, localContact.Normal, GetTransform()->GetWorldMatrix() );						
					}

					return true;
				}
			}
			break;
		case EDMath::BoundingVolume::AABB:
			{
			}
			break;
		case EDMath::BoundingVolume::OBB:
			{
				Obb worldObb = (const Obb&)boundingVolume;
				const Float4x4& bvhWorld = GetTransform()->GetWorldMatrix();

				Obb localObb; 
				EDMath::InvTransformObb( localObb, worldObb, bvhWorld );

				EDCollision::Contact localContact;
				localContact.PenetrationDepth = -FLT_MAX;
				unsigned int triIndex = 0xFFFFFFFF;

				//ObbToBVH( localObb, bvhPtr->getRoot(), bvhPtr, localContact, triIndex );

				//if( localContact.PenetrationDepth > 0.0f )
				
				if( bvhPtr->Collide( localObb, triIndex, &localContact ) )
				{
					if( contact )
					{
						const EDMath::WingedTriangle* tris = bvhPtr->getTriangles();
						const Float3* verts = bvhPtr->getVertices();
						const EDMath::WingedTriangle& tri = tris[triIndex];

						TransformVector( contact->Normal, localContact.Normal, bvhWorld );
						contact->PenetrationDepth = localContact.PenetrationDepth;

						Float3 cd = (contact->Normal * contact->PenetrationDepth * 0.95f);

						Float3 v[3];
						for(int i = 0; i < 3; ++i)
						{
							TransformPoint( v[i], verts[ tri.indices[i] ], bvhWorld );

							v[i] -= cd;
						}


						ClipTriangleObb( worldObb, v[0], v[1], v[2], contact->Point, contact->numPoints);

						return contact->numPoints != 0;
					}

					return true;
				}
			}
			break;
		default:
			break;
		}

		return false;
	}

	const EDMath::Aabb& BVHCollider::GetAabb(void)
	{
		GameObject* gameObject = GetGameObject();
		const Float4x4& worldMat = gameObject->GetTransform()->GetWorldMatrix();
		
		Aabb box;
		box.center = box.extents.makeZero();

		if( bvhPtr != 0 )
		{
			box = bvhPtr->getRoot()->getBounds();
		}

		for(int i = 0; i < 3; ++i)
		{
			worldAabb.center.v[i] = worldMat.WAxis.v[i];
			worldAabb.extents.v[i] = 0.0f;

			for(int j = 0; j < 3; ++j)
			{
				worldAabb.center.v[i] += worldMat.m[j][i] * box.center.v[j];
				worldAabb.extents.v[i] += abs(worldMat.m[j][i]) * box.extents.v[j];
			}
		}

		return worldAabb;		
	}
}
