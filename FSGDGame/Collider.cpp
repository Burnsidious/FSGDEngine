#include "Collider.h"

#include "../EDCollision/Collision.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDUtilities/GDMeshStream.h"
#include "../EDRendererD3D/DebugRenderer.h"

#include "../EDUtilities/XMLLoadingMethods.h"

using namespace EDGOCA;
using namespace EDCollision;
using namespace EDMath;
using namespace EDUtilities;
using namespace GameObjectMessages;
using std::pair;
using std::string;

namespace ColliderMessages
{
	DeclareMessage(MSG_GET_COLLIDER);
	DeclareMessage( MSG_COLLIDE );
	DeclareMessage( MSG_SET_COLLIDER_MESH );
	DeclareMessage( MSG_APPLY_FORCE );
	DeclareMessage( MSG_APPLY_REACTION );
	DeclareMessage( MSG_INTERSECT_SEGMENT );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
}

using namespace Behaviors;

struct ColliderDescriptor
{
	Collider::ColliderType type;
	Collider::ColliderObject* object;

	ColliderDescriptor(void)
	{
		type = Collider::INVALID;
		object = nullptr;
	}

	~ColliderDescriptor()
	{
		delete object;
	}

	ColliderDescriptor& operator=(const ColliderDescriptor& rhs)
	{
		if( this != &rhs )
		{
			delete object;
			type = rhs.type;
			
			if( rhs.object != 0 )
				object = rhs.object->clone();
			else
				object = 0;
		}

		return *this;
	}

	ColliderDescriptor& operator=( ColliderDescriptor&& rhs)
	{
		if( this != &rhs )
		{
			delete object;
			type = std::move(rhs.type);
			object = std::move(rhs.object);
		
			rhs.type = Collider::INVALID;
			rhs.object = nullptr;
		}
		return *this;
	}
};

template<>
void Attribute<ColliderDescriptor>::ReadXML(TiXmlElement* valElement)
{
	TiXmlElement* pTypeEle = valElement->FirstChildElement("Type");

	if( pTypeEle == 0 )
		return;

	if( strcmp( pTypeEle->GetText(), "AABB" ) == 0 )
	{
		delete value.object;
		value.object = new Collider::AabbColliderObject();
		value.type = Collider::AABB;
		value.object->ReadXML( valElement );
	}
	else if( strcmp( pTypeEle->GetText(), "OBB" ) == 0 )
	{
		delete value.object;
		value.object = new Collider::ObbColliderObject();
		value.type = Collider::OBB;
		value.object->ReadXML( valElement );
	}
	else if( strcmp( pTypeEle->GetText(), "SPHERE" ) == 0 )
	{
		delete value.object;
		value.object = new Collider::SphereColliderObject();
		value.type = Collider::SPHERE;
		value.object->ReadXML( valElement );
	}
	else if( strcmp( pTypeEle->GetText(), "CAPSULE" ) == 0 )
	{
		delete value.object;
		value.object = new Collider::CapsuleColliderObject();
		value.type = Collider::CAPSULE;
		value.object->ReadXML( valElement );
	}
	else if( strcmp( pTypeEle->GetText(), "MESH" ) == 0 )
	{
		delete value.object;
		value.object = new Collider::MeshColliderObject();
		value.type = Collider::MESH;
		value.object->ReadXML( valElement );
	}
}

namespace ColliderAttributes
{
	DeclareAttribute( ColliderDescriptor, COLLIDER_ATTRIB );
}

namespace Behaviors
{
	void SphereToTriangle(const Float3& v0, const Float3& v1, const Float3& v2, const Float3& n, BVHTraversalStruct &traversalStruct)
	{
		pair<bool, EDCollision::Sphere>* spherePair = (pair<bool, EDCollision::Sphere>*)traversalStruct.dataPtr;
		EDCollision::Sphere& sphere = spherePair->second;

		Float3 vtop = sphere.center - v0;
		float d = DotProduct( vtop, n );

		if( d < 0.0f )
			return;
		if( abs(d) > sphere.radius )
			return;

		Float3 projPt = sphere.center - n * d;
		Float3 edgeStart = v0;
		Float3 edge = v1 - v0;
		Float3 edgeNorm;
		CrossProduct( edgeNorm, edge, n );
		vtop = projPt - v0;
		if( DotProduct( vtop, edgeNorm ) <= 0.0f )
		{
			edgeStart = v1;
			edge = v2 - v1;
			CrossProduct( edgeNorm, edge, n );
			vtop = projPt - v1;
			
			if( DotProduct( vtop, edgeNorm ) <= 0.0f )
			{
				edgeStart = v2;
				edge = v0 - v2;
				CrossProduct( edgeNorm, edge, n );
				vtop = projPt - v2;
				
				if( DotProduct( vtop, edgeNorm ) <= 0.0f )
				{
					sphere.center += n * (sphere.radius - d);
					spherePair->first = true;
					return;
				}
			}
		}

		float l = DotProduct( edge, edge );
		float v = DotProduct( vtop, edge );

		float ratio = v / l;
		if( ratio < 0.0f )
			ratio = 0.0f;
		if( ratio > 1.0f )
			ratio = 1.0f;

		Float3 cp = edgeStart + edge * ratio;
		
		edgeNorm = sphere.center - cp;
		float dsq = DotProduct( edgeNorm, edgeNorm );
		
		if( dsq > sphere.radius*sphere.radius )
			return;

		if( dsq < FLT_EPSILON )
		{
			d = 0.0f;
			edgeNorm = n;
		}
		else
		{
			d = sqrt(dsq);
			edgeNorm *= (1.0f / d);
		}

		sphere.center += edgeNorm * (sphere.radius - d);
		spherePair->first = true;
	}

	bool SphereToNodeAabb(const Aabb& nodeAabb, BVHTraversalStruct &traversalStruct)
	{
		pair<bool, EDCollision::Sphere>* spherePair = (pair<bool, EDCollision::Sphere>*)traversalStruct.dataPtr;
		EDCollision::Sphere& sphere = spherePair->second;
		
		Float3 closest = sphere.center;
		for(unsigned int i = 0; i < 3; ++i)
		{
			if( closest.v[i] > nodeAabb.max.v[i] )
				closest.v[i] = nodeAabb.max.v[i];
			if( closest.v[i] < nodeAabb.min.v[i] )
				closest.v[i] = nodeAabb.min.v[i];
		}

		Float3 v = closest - sphere.center;

		bool breturn = ( DotProduct( v, v ) < sphere.radius*sphere.radius );

		//if( breturn )
			//DebugRenderer::GetInstance()->DrawAabb(nodeAabb);

		return breturn;
	}

	bool SegmentToNodeAabb(const Aabb& nodeAabb, BVHTraversalStruct &traversalStruct)
	{
		pair<bool, Segment>* segPair = (pair<bool, Segment>*)traversalStruct.dataPtr;
		Segment& segment = segPair->second;

		return AABBSegmentCollide( segment.start, segment.end, nodeAabb.min, nodeAabb.max );
	}

	void SegmentToTriangle(const Float3& v0, const Float3& v1, const Float3& v2, const Float3& n, BVHTraversalStruct &traversalStruct)
	{
		pair<bool, Segment>* segPair = (pair<bool, Segment>*)traversalStruct.dataPtr;
		Segment& segment = segPair->second;

		float t = FLT_MAX;
		if( SegmentTriangleIntersect( v0, v1, v2, n, segment.start, segment.end, t ) )
		{
			segment.end = segment.start + (segment.end - segment.start) * t;
			segPair->first = true;
		}
	}

	BVHTemplate* BVHTemplate::Load(const char* pGDMeshFile, const char* indentifier)
	{
		string temp = ContentManager::theContentPath;
		temp += pGDMeshFile;
		const char* xmlFileNamePtr = temp.c_str();

		ContentHandle<GDMeshStreamBinary> meshHandle = ContentManager::Load<GDMeshStreamBinary>(xmlFileNamePtr);
	
		ContentHandle<GDAttribute<Point>> pointsHandle = meshHandle.GetContent()->ReadAttribute<Point>();
		if( pointsHandle.GetContent() == 0 )
			return 0;

		size_t numVerts = pointsHandle.GetContent()->size();
		if( numVerts == 0 )
			return 0;

		ContentHandle<GDAttribute<TriVertIndices>> indicesHandle = meshHandle.GetContent()->GetIndicesHandle();
		if( indicesHandle.GetContent() == 0 )
			return 0;

		ContentHandle<GDAttribute<TriNormal>> triNormalHandle = meshHandle.GetContent()->ReadAttribute<TriNormal>();

		if( triNormalHandle.GetContent() == 0 )
			return 0;

		ContentHandle<GDAttribute<BVHNode>> bvhNodesHandle = meshHandle.GetContent()->ReadAttribute<BVHNode>();

		if( bvhNodesHandle.GetContent() == 0 )
			return 0;

		BVHTemplate* resultPtr = new BVHTemplate;
		resultPtr->aabbBvhPtr = new AabbBvh;
		resultPtr->m_PointsHandle = std::move(pointsHandle);
		resultPtr->triNormalsHandle = std::move(triNormalHandle);
		resultPtr->indicesHandle = std::move(indicesHandle);

		EDTreeNode** pNodeArr = new EDTreeNode*[bvhNodesHandle.GetContent()->size()];

		for(unsigned int i = 0; i < bvhNodesHandle.GetContent()->size(); ++i)
		{
			BVHNode& currentNode = bvhNodesHandle.GetContent()->operator[](i);

			if( currentNode.isLeaf == false )
			{
				AabbBvhInternalNode* pNewInternal = new AabbBvhInternalNode;
				BvhAabb* pNewInternalData = new BvhAabb;
				pNewInternalData->min = currentNode.AABB.min;
				pNewInternalData->max = currentNode.AABB.max;
				pNewInternal->SetInternalNodeData(pNewInternalData);
				pNodeArr[i] = pNewInternal;

				for(unsigned int childIndex = 0; childIndex < currentNode.indices.size(); ++childIndex)
					pNodeArr[ currentNode.indices[childIndex] ]->SetParent( pNodeArr[i] );
			}
			else
			{
				AabbBvhLeafNode::AabbBvhLeafData* newLeafDataPtr = new AabbBvhLeafNode::AabbBvhLeafData(&currentNode.indices[0], (unsigned int)currentNode.indices.size(), currentNode.AABB);
				AabbBvhLeafNode* newLeafPtr = newLeafDataPtr->CreateLeafNode(resultPtr->aabbBvhPtr);
				pNodeArr[i] = newLeafPtr;
			}
		}

		resultPtr->aabbBvhPtr->SetRootNode( (EDTreeInternalNode*)pNodeArr[bvhNodesHandle.GetContent()->size()-1] );

		std::vector< EDTreeLeafNode* >& leafList = resultPtr->aabbBvhPtr->GetLeaves();
	
		for( unsigned int leafNumber = 0; leafNumber < leafList.size(); ++leafNumber )
			((AabbBvhLeafNode::AabbBvhLeafData*)(leafList[leafNumber])->GetLeafNodeData())->SetLeafNumber( leafNumber );

	
		delete [] pNodeArr;

		return resultPtr;	
	}

	void BVHTemplate::Traverse( BVHTraversalStruct& traversalStruct )
	{
		Traverse( aabbBvhPtr->GetRoot(), traversalStruct );
	}

	void BVHTemplate::Traverse( const EDTreeNode* node, BVHTraversalStruct& traversalStruct )
	{
		if( node->IsLeaf() )
		{
			const AabbBvhLeafNode* leafPtr = (const AabbBvhLeafNode*)node;
			if( traversalStruct.perNodeFunc( *leafPtr->GetBounds(), traversalStruct ) )
			{
				AabbBvhLeafNode::AabbBvhLeafData* leafDataPtr = leafPtr->GetLeafNodeData();
				unsigned int triCount = leafDataPtr->GetNumTriangleIndices();
				unsigned int* triIndices = leafDataPtr->GetTriangleIndices();

				for(unsigned int i = 0; i < triCount; ++i)
				{
					unsigned int triIndex = triIndices[i];
					
					traversalStruct.perTriangleFunc( m_PointsHandle.GetContent()->operator[](indicesHandle.GetContent()->operator[]( triIndex ).index0),
													   m_PointsHandle.GetContent()->operator[](indicesHandle.GetContent()->operator[]( triIndex ).index1),
													   m_PointsHandle.GetContent()->operator[](indicesHandle.GetContent()->operator[]( triIndex ).index2),
													   triNormalsHandle.GetContent()->operator[]( triIndex ),
													   traversalStruct );
				}
			}
		}
		else
		{
			const AabbBvhInternalNode* pInternal = (const AabbBvhInternalNode*)node;
			if( traversalStruct.perNodeFunc( *pInternal->GetInternalNodeData(), traversalStruct ) )
			{
				Traverse( pInternal->GetChildren(), traversalStruct );
				Traverse( pInternal->GetChildren()->GetNextSibling(), traversalStruct );
			}
		}
	}

	void BVHTemplate::GetLocalAabb(Aabb& aabb)const
	{
		if( aabbBvhPtr != 0 )
			aabbBvhPtr->GetLocalAabb(aabb);
	}


	Collider::~Collider(void)
	{
	}

	const AttributeNameKey* Collider::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			ColliderAttributes::COLLIDER_ATTRIB,
			0
		};

		return attribs;
	}

	void Collider::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( ColliderMessages::MSG_COLLIDE, OnCollide ) );
		AddMessageHandler( ColliderMessages::MSG_GET_COLLIDER, OnGetCollider );
		AddMessageHandler( MessageHandler( ColliderMessages::MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
		AddMessageHandler( MSG_ATTRIBUTE_CHANGED, OnAttributeChanged );
		AddMessageHandler( ColliderMessages::MSG_INTERSECT_SEGMENT, OnIntersectSegment );
	}

	void Collider::OnBuildLocalAabb( IBehavior* invokingBehavior, IMessage* msg )
	{
		Collider* pThis = (Collider*)invokingBehavior;
		MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

		if( pThis->colliderObject != 0 )
		{
			Aabb aabb;
			aabb.min = Float3( FLT_MAX, FLT_MAX, FLT_MAX );
			aabb.max = Float3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

			pThis->colliderObject->GetLocalAabb(aabb);
			
			for(int i = 0; i < 3; ++i)
			{
				if( message->value->min.v[i] > aabb.min.v[i] )
					message->value->min.v[i] = aabb.min.v[i];
				if( message->value->max.v[i] < aabb.max.v[i] )
					message->value->max.v[i] = aabb.max.v[i];
			}
		}
	}

	void Collider::OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		Collider* pThis = (Collider*)pInvokingBehavior;
		MessageT<AttributeNameKey>* message = (MessageT<AttributeNameKey>*)msg;

		if( message->value == ColliderAttributes::COLLIDER_ATTRIB )
		{
			pThis->colliderObject = ((Attribute<ColliderDescriptor>*)pThis->GetAttribute(
				ColliderAttributes::COLLIDER_ATTRIB))->value.object;
			pThis->gameObject->BuildLocalAabb();
		}
	}

	void Collider::OnGetCollider( IBehavior* invokingBehavior, IMessage* message )
	{
		MessageT<Collider*>* msg = (MessageT<Collider*>*)message;
		msg->value = (Collider*)invokingBehavior;
	}

	void Collider::OnIntersectSegment( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		Collider* pThis = (Collider*)pInvokingBehavior;
		MessageT< pair<bool, Segment> >* message = (MessageT< pair<bool, Segment> >*)msg;

		if( pThis->GetColliderType() == MESH )
		{
			pair<bool, Segment> localSegPair( false, message->value.second );
			localSegPair.second.InverseTransform( pThis->gameObject->GetWorldTransform() );

			BVHTraversalStruct traversalStruct;
			traversalStruct.perTriangleFunc = SegmentToTriangle;
			traversalStruct.perNodeFunc = SegmentToNodeAabb;
			traversalStruct.dataPtr = &localSegPair;

			((MeshColliderObject*)pThis->colliderObject)->Traverse( traversalStruct );
			
			if( localSegPair.first == true )
			{
				message->value.first = true;
				TransformPoint( message->value.second.end, localSegPair.second.end, pThis->gameObject->GetWorldTransform() );
			}
		}
	}

	void Collider::OnCollide( IBehavior* invokingBehavior, IMessage* message )
	{
		MessageT<GameObject*>* msg = (MessageT<GameObject*>*)message;

		MessageT<Collider*> getColliderMsg(ColliderMessages::MSG_GET_COLLIDER, 0);

		msg->value->OnMessage( &getColliderMsg );

		if( getColliderMsg.value == 0 )
			return;

		Collider* pThis = (Collider*)invokingBehavior;
		Collider* pOther = getColliderMsg.value;
		
		bool result = false;
		CollisionContact collisionContact;

		switch(pThis->GetColliderType())
		{
		case AABB:
			{
				switch(pOther->GetColliderType())
				{
				case AABB:
					{
						result = ColliderAabbToAabb(pThis, pOther, collisionContact);
					}
					break;
				case OBB:
					{
						result = ColliderAabbToObb(pThis, pOther, collisionContact);
					}
					break;
				case SPHERE:
					{
						result = ColliderAabbToSphere(pThis, pOther, collisionContact);
					}
					break;
				case CAPSULE:
					{
						result = ColliderAabbToCapsule(pThis, pOther, collisionContact);
					}
					break;
				case MESH:
					{
						result = ColliderAabbToMesh(pThis, pOther, collisionContact);
					}
					break;
				default:
					break;
				}
			}
			break;
		case OBB:
			{
				switch(pOther->GetColliderType())
				{
				case AABB:
					{
						result = ColliderAabbToObb(pOther, pThis, collisionContact);
					}
					break;
				case OBB:
					{
						result = ColliderObbToObb(pThis, pOther, collisionContact);
					}
					break;
				case SPHERE:
					{
						result = ColliderObbToSphere(pThis, pOther, collisionContact);
					}
					break;
				case CAPSULE:
					{
						result = ColliderObbToCapsule(pThis, pOther, collisionContact);
					}
					break;
				case MESH:
					{
						result = ColliderObbToMesh(pThis, pOther, collisionContact);
					}
					break;
				default:
					break;
				}
			}
			break;
		case SPHERE:
			{
				switch(pOther->GetColliderType())
				{
				case AABB:
					{
						result = ColliderAabbToSphere(pOther, pThis, collisionContact);
					}
					break;
				case OBB:
					{
						result = ColliderObbToSphere(pOther, pThis, collisionContact);
					}
					break;
				case SPHERE:
					{
						result = ColliderSphereToSphere(pThis, pOther, collisionContact);
					}
					break;
				case CAPSULE:
					{
						result = ColliderSphereToCapsule(pThis, pOther, collisionContact);
					}
					break;
				case MESH:
					{
						result = ColliderSphereToMesh(pThis, pOther, collisionContact);
					}
					break;
				default:
					break;
				}
			}
			break;
		case CAPSULE:
			{
				switch(pOther->GetColliderType())
				{
				case AABB:
					{
						result = ColliderAabbToCapsule(pOther, pThis, collisionContact);
					}
					break;
				case OBB:
					{
						result = ColliderObbToCapsule(pOther, pThis, collisionContact);
					}
					break;
				case SPHERE:
					{
						result = ColliderSphereToCapsule(pOther, pThis, collisionContact);
					}
					break;
				case CAPSULE:
					{
						result = ColliderCapsuleToCapsule(pThis, pOther, collisionContact);
					}
					break;
				case MESH:
					{
						result = ColliderCapsuleToMesh(pThis, pOther, collisionContact);
					}
					break;
				default:
					break;
				}

			}
			break;
		case MESH:
			{
				switch(pOther->GetColliderType())
				{
				case AABB:
					{
						result = ColliderAabbToMesh(pOther, pThis, collisionContact);
					}
					break;
				case OBB:
					{
						result = ColliderObbToMesh(pOther, pThis, collisionContact);
					}
					break;
				case SPHERE:
					{
						result = ColliderSphereToMesh(pOther, pThis, collisionContact);
					}
					break;
				case CAPSULE:
					{
						result = ColliderSphereToMesh(pOther, pThis, collisionContact);
					}
					break;
				case MESH:
					{
						result = ColliderMeshToMesh(pThis, pOther, collisionContact);
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

		if( result == false )
			return;

		if( pThis->GetColliderType() > pOther->GetColliderType() )
		{
			collisionContact.m_ContactPoint += collisionContact.m_ContactNormal * collisionContact.m_PenetrationDepth;
			collisionContact.m_ContactNormal.negate();
		}

		pThis->gameObject->TranslateGlobal( collisionContact.m_ContactNormal * collisionContact.m_PenetrationDepth );
	}

	Collider::ColliderType Collider::GetColliderType(void)const
	{
		if( colliderObject == 0 )
			return INVALID;
		
		return colliderObject->GetColliderType();
	}

	bool Collider::ColliderAabbToAabb(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderAabbToObb(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderAabbToSphere(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderAabbToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderAabbToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderObbToObb(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderObbToSphere(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderObbToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderObbToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}
		
	bool Collider::ColliderSphereToSphere(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderSphereToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderSphereToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		SphereColliderObject* pSphereObj = (SphereColliderObject*)objA->GetColliderObject();
		MeshColliderObject* pMeshObj = (MeshColliderObject*)objB->GetColliderObject();

		EDCollision::Sphere worldSphere = pSphereObj->GetLocalSphere();
		worldSphere.Transform( objA->gameObject->GetWorldTransform() );

		EDCollision::Sphere objSpaceSphere = worldSphere;
		objSpaceSphere.InverseTransform( objB->gameObject->GetWorldTransform() );

		BVHTraversalStruct traversalStruct;
		pair<bool, EDCollision::Sphere> spherePair( false, objSpaceSphere );
		traversalStruct.perNodeFunc = SphereToNodeAabb;
		traversalStruct.perTriangleFunc = SphereToTriangle;
		traversalStruct.dataPtr = &spherePair;

		pMeshObj->Traverse(traversalStruct);

		//DebugRenderer::GetInstance()->DrawSphere( spherePair.second );

		if( spherePair.first == false )
			return false;

		collisionContact.m_ContactNormal = spherePair.second.center - objSpaceSphere.center;

		if( DotProduct( collisionContact.m_ContactNormal, collisionContact.m_ContactNormal ) < ED_EPSILON )
			return false;

		Float3 tempV;
		TransformVector(tempV, collisionContact.m_ContactNormal, objB->gameObject->GetWorldTransform() );
		collisionContact.m_ContactNormal = tempV;

		collisionContact.m_PenetrationDepth = collisionContact.m_ContactNormal.magnitude();
		collisionContact.m_ContactNormal *= (1.0f / collisionContact.m_PenetrationDepth );
		
		collisionContact.m_ContactPoint = worldSphere.center - collisionContact.m_ContactNormal * worldSphere.radius;

		return true;
	}

	bool Collider::ColliderCapsuleToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderCapsuleToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	bool Collider::ColliderMeshToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact)
	{
		return false;
	}

	void Collider::AabbColliderObject::GetLocalAabb(Aabb& aabb)
	{
		aabb = localAabb;
	}

	void Collider::AabbColliderObject::ReadXML(TiXmlElement* objectEle)
	{
	}

	void Collider::ObbColliderObject::GetLocalAabb(Aabb& aabb)
	{
		for(int i = 0; i < 3; ++i)
		{
			aabb.min.v[i] = aabb.max.v[i] = localObb.transform.WAxis.v[i];

			for(int j = 0; j < 3; ++j)
			{
				float f = localObb.transform.m[i][j] * localObb.extents.v[j];
				float e = -f;

				if( e < f )
				{
					aabb.min.v[i] += e;
					aabb.max.v[i] += f;
				}
				else
				{
					aabb.min.v[i] += f;
					aabb.max.v[i] += e;
				}
			}
		}
	}

	void Collider::ObbColliderObject::ReadXML(TiXmlElement* objectEle)
	{
	}

	void Collider::SphereColliderObject::GetLocalAabb(Aabb& aabb)
	{
		aabb.min.x = localSphere.center.x - localSphere.radius;
		aabb.min.y = localSphere.center.y - localSphere.radius;
		aabb.min.z = localSphere.center.z - localSphere.radius;

		aabb.max.x = localSphere.center.x + localSphere.radius;
		aabb.max.y = localSphere.center.y + localSphere.radius;
		aabb.max.z = localSphere.center.z + localSphere.radius;
	}

	void Collider::SphereColliderObject::ReadXML(TiXmlElement* objectEle)
	{
		TiXmlElement* posEle = objectEle->FirstChildElement("Center");
		ReadXmlVector4( posEle, localSphere.center.v );
		
		TiXmlElement* radEle = objectEle->FirstChildElement("Radius");
		localSphere.radius = (float)atof( radEle->GetText() );
	}

	void Collider::CapsuleColliderObject::GetLocalAabb(Aabb& aabb)
	{
		aabb.min.x = aabb.min.y = aabb.min.z = FLT_MAX;
		aabb.max.x = aabb.max.y = aabb.max.z = -FLT_MAX;

		for(unsigned int i = 0; i < 3; ++i)
		{
			if( aabb.max.v[i] < localCapsule.segment.start.v[i] + localCapsule.radius )
				aabb.max.v[i] = localCapsule.segment.start.v[i] + localCapsule.radius; 
			if( aabb.min.v[i] > localCapsule.segment.start.v[i] - localCapsule.radius )
				aabb.min.v[i] = localCapsule.segment.start.v[i] - localCapsule.radius; 

			if( aabb.max.v[i] < localCapsule.segment.end.v[i] + localCapsule.radius )
				aabb.max.v[i] = localCapsule.segment.end.v[i] + localCapsule.radius; 
			if( aabb.min.v[i] > localCapsule.segment.end.v[i] - localCapsule.radius )
				aabb.min.v[i] = localCapsule.segment.end.v[i] - localCapsule.radius; 
		}
	}

	void Collider::CapsuleColliderObject::ReadXML(TiXmlElement* objectEle)
	{
	}

	void Collider::MeshColliderObject::GetLocalAabb(Aabb& aabb)
	{
		BVHTemplate* bvhTemplate = bvhHandle.GetContent();

		if( bvhTemplate != 0 )
			bvhTemplate->GetLocalAabb(aabb);
	}

	void Collider::MeshColliderObject::ReadXML(TiXmlElement* objectEle)
	{
		TiXmlElement* meshPathEle = objectEle->FirstChildElement("Path");
		
		const char* path = meshPathEle->GetText();

		bvhHandle = ContentManager::Load<BVHTemplate>( meshPathEle->GetText() ); 
	}

	void Collider::MeshColliderObject::Traverse(BVHTraversalStruct& traversalStruct)
	{
		BVHTemplate* bvhTemplate = bvhHandle.GetContent();

		if( bvhTemplate != 0 )
			bvhTemplate->Traverse( traversalStruct );		
	}
}