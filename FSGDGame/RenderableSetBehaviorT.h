#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDGOCA/GameObject.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDRendererD3D/RenderShape.h"

const UINT globalNumLODRadii = 2;
const float globalLODRadii[globalNumLODRadii] = { 0.01f, 0.005f };
const float globalHysteresis = 0.001f;

namespace Behaviors
{
	typedef unsigned int RenderShapeKey;

	typedef std::vector< std::string > RenderShapeLODPaths;
	typedef std::list< std::pair< RenderShapeLODPaths, EDGOCA::TransformKey > > RenderShapePathList;

	typedef std::list< std::pair< EDRendererD3D::RenderShape *, EDGOCA::TransformKey > > RenderShapeList;

	template<class DerivedRenderable>
	class RenderableSetBehaviorT : public EDGOCA::BehaviorT<DerivedRenderable>
	{
	protected:
		RenderShapeList renderShapeList;

	public:
		RenderableSetBehaviorT(void) {}

		virtual ~RenderableSetBehaviorT(void)
		{
			while( !renderShapeList.empty() )
			{
				delete renderShapeList.front().first;

				renderShapeList.pop_front();
			}
		}

		static void OnSetRenderShapeList( EDGOCA::IBehavior* invokingBehavior, 
			EDGOCA::IMessage* iMsg )
		{
			RenderableSetBehaviorT* pThis = (RenderableSetBehaviorT*)invokingBehavior;

			MessageT< RenderShapePathList* >* message = (MessageT< RenderShapePathList* >*)iMsg;

			while( !pThis->renderShapeList.empty() )
			{
				delete pThis->renderShapeList.front().first;

				pThis->renderShapeList.pop_front();
			}
			
			RenderShapePathList* renderShapePathList = message->value;
			
			RenderShapePathList::iterator shapePathIter = renderShapePathList->begin();
			for(; shapePathIter != renderShapePathList->end(); ++shapePathIter )
			{
				std::vector< EDUtilities::ContentHandle<RenderForm> > formVector;
				for(unsigned int i = 0; i < shapePathIter->first.size(); ++i)
				{
					EDUtilities::ContentHandle<RenderForm> formHandle = 
						EDUtilities::ContentManager::LoadXML<RenderForm>( 
						shapePathIter->first[i].c_str() );
					
					if( formHandle.GetContent() != 0 )
						formVector.push_back( formHandle );
				}

				if( !formVector.empty() )
				{
					pair< RenderShape*, TransformKey > newShapePair;
					newShapePair.second = shapePathIter->second;

					newShapePair.first = (RenderShape*)formVector[0].GetContent()->GetNewRenderShape();
					newShapePair.first->Initialize( formVector );
					newShapePair.first->SetWorldMatrix( (DirectX::XMFLOAT4X4*)&pThis->gameObject->GetWorldTransform( shapePathIter->second ) );

					for(unsigned int i = 0; i < formVector.size(); ++i )
					{
						RenderContext* pContext = newShapePair.first->GetRenderContext(i);
						RenderController::GetInstance()->AddRenderContext( pContext );
					}

					pThis->renderShapeList.push_back( newShapePair );
				}
			}

			pThis->gameObject->BuildLocalAabb();
		}

		static void OnFrustumTest( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg )
		{
			RenderableSetBehaviorT* pThis = (RenderableSetBehaviorT*)pInvokingBehavior;
			MessageT<EDCollision::FrustumHull*>* message = (MessageT<EDCollision::FrustumHull*>*)msg;

			EDCollision::Aabb tempAabb = pThis->gameObject->GetAabb();

			if( message->value->Collides( &tempAabb ) )
			{
				SetLOD(pThis->renderShapeList, *pThis->gameObject);
			}
		}
		static void SetLODSolution( RenderShapeList &shapeList, EDGOCA::GameObject &gameObject);

		static void SetLOD( RenderShapeList &shapeList, EDGOCA::GameObject &gameObject)
		{
			SetLODSolution(shapeList, gameObject);
		}

		static void OnBuildLocalAabb( EDGOCA::IBehavior* invokingBehavior, EDGOCA::IMessage* msg )
		{
			RenderableSetBehaviorT* pThis = (RenderableSetBehaviorT*)invokingBehavior;
			MessageT<EDCollision::Aabb*>* message = (MessageT<EDCollision::Aabb*>*)msg;

			RenderShapeList::iterator shapeListIter = pThis->renderShapeList.begin();
			for(; shapeListIter != pThis->renderShapeList.end(); ++shapeListIter)
			{
				EDCollision::Aabb tempAabb = shapeListIter->first->GetRenderMesh()->GetAabb();

				for(unsigned int i = 0; i < 3; ++i)
				{
					if( message->value->min.v[i] > tempAabb.min.v[i] )
						message->value->min.v[i] = tempAabb.min.v[i];
					if( message->value->max.v[i] < tempAabb.max.v[i] )
						message->value->max.v[i] = tempAabb.max.v[i];
				}
				//Sphere tempSphere = shapeListIter->first->GetLocalSphere();

				//const Float4x4& xform = pThis->gameObject->GetLocalTransform( shapeListIter->second );

				//if( xform != pThis->gameObject->GetLocalTransform() )
				//	tempSphere.Transform( xform );

				//for(unsigned int i = 0; i < 3; ++i)
				//{
				//	if( message->value->min.v[i] > tempSphere.center.v[i] - tempSphere.radius )
				//		message->value->min.v[i] = tempSphere.center.v[i] - tempSphere.radius;

				//	if( message->value->max.v[i] < tempSphere.center.v[i] + tempSphere.radius )
				//		message->value->max.v[i] = tempSphere.center.v[i] + tempSphere.radius;
				//}
			}
		}

		static void OnGetRenderableCount( EDGOCA::IBehavior* pInvokingBehavior, 
			EDGOCA::IMessage* msg )
		{
			MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
			message->value += 1;
		}
	};
}