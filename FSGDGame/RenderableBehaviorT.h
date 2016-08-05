#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/RenderShape.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"

namespace Behaviors
{
	template<class DerivedRenderable>
	class RenderableBehaviorT : public EDGOCA::BehaviorT<DerivedRenderable>
	{
		EDRendererD3D::RenderShapeBase* renderShape;

	public:
		RenderableBehaviorT(void) : renderShape(0) {}

		virtual ~RenderableBehaviorT(void)
		{
			delete renderShape;
		}

		static void OnSetRenderShape( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg )
		{
			RenderableBehaviorT* pRenderable = (RenderableBehaviorT*)pInvokingBehavior;
			MessageT< std::pair<BehaviorNameKey, std::string> >* message = (MessageT< std::pair<BehaviorNameKey, std::string> >*)msg;

			if( message->value.first != pRenderable->GetBehaviorNameKey() )
				return;

			if( pRenderable->renderShape != 0 )
			{
				delete pRenderable->renderShape;
				pRenderable->renderShape = 0;
			}

			if( message->value.second.length() != 0 )
			{
				std::vector< EDUtilities::ContentHandle<EDRendererD3D::RenderForm> > formVector;
				EDUtilities::ContentHandle<EDRendererD3D::RenderForm> formHandle = 
					EDUtilities::ContentManager::LoadXML<EDRendererD3D::RenderForm>( 
					message->value.second.c_str() );

				if( formHandle.GetContent() != 0 )
				{
					formVector.push_back( formHandle );
				
					pRenderable->renderShape = formHandle.GetContent()->GetNewRenderShape();
//					pRenderable->renderShape->Initialize( formVector );
//					pRenderable->renderShape->SetWorldMatrix( (D3DXMATRIX*)&pRenderable->gameObject->GetWorldTransform() );

					RenderContext* pContext = pRenderable->renderShape->GetRenderContext();
					RenderController::GetInstance()->AddRenderContext( pContext );
				}
			}

			pRenderable->gameObject->BuildLocalAabb();
		}

		static void OnFrustumTest( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg )
		{
			RenderableBehaviorT* pRenderable = (RenderableBehaviorT*)pInvokingBehavior;
			MessageT<EDCollision::FrustumHull*>* message = (MessageT<EDCollision::FrustumHull*>*)msg;

			if( pRenderable->renderShape == 0 )
				return;

			EDCollision::Aabb tempAabb = pRenderable->gameObject->GetAabb();
			if( message->value->Collides( &tempAabb ) )
			{
//				pRenderable->renderShape->SetWorldMatrix( (D3DXMATRIX*)&pRenderable->gameObject->GetWorldTransform() );
				pRenderable->renderShape->AddToContextSet();
			}
		}

		static void OnBuildLocalAabb( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg )
		{
			RenderableBehaviorT* pRenderable = (Renderable*)pInvokingBehavior;
			MessageT<EDCollision::Aabb*>* message = (MessageT<EDCollision::Aabb*>*)msg;

			if( pRenderable->renderShape == 0 )
				return;

			const EDCollision::Aabb& localAabb = pRenderable->renderShape->GetRenderMesh()->GetAabb();
			for(unsigned int i = 0; i < 3; ++i)
			{
				if( message->value->min.v[i] > localAabb.min.v[i] )
					message->value->min.v[i] = localAabb.min.v[i];
				if( message->value->max.v[i] < localAabb.max.v[i] )
					message->value->max.v[i] = localAabb.max.v[i];
			}
		}

		static void OnGetRenderableCount( EDGOCA::IBehavior* pInvokingBehavior, EDGOCA::IMessage* msg )
		{
			MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
			message->value += 1;
		}
	};
}