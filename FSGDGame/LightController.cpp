#include "LightController.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/RenderModel.h"
#include "../EDRendererD3D/RenderShape.h"

using namespace EDRendererD3D;
using namespace EDCollision;
using namespace EDGOCA;

namespace LightControllerMessages
{
	DeclareMessage( MSG_SET_LIGHT );
	DeclareMessage( MSG_SET_LIGHT_CONTEXT );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
}

namespace Behaviors
{
	void LightController::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( LightControllerMessages::MSG_SET_LIGHT, OnSetLight ) );
		AddMessageHandler( MessageHandler( LightControllerMessages::MSG_SET_LIGHT_CONTEXT, OnSetRenderContext ) );
		AddMessageHandler( MessageHandler( LightControllerMessages::MSG_FRUSTUM_TEST, OnFrustumTest ) );
		AddMessageHandler( MessageHandler( LightControllerMessages::MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
	}

	void LightController::OnSetLight( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		LightController* lightBehaviorPtr = (LightController*)pInvokingBehavior;
		MessageT<ILight *>* message = (MessageT<ILight *>*)msg;

		lightBehaviorPtr->lightPtr = (ILight *)message->value;

		lightBehaviorPtr->gameObject->BuildLocalAabb();
	}

	void LightController::OnSetRenderContext( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		LightController* lightBehaviorPtr = (LightController*)pInvokingBehavior;
		MessageT<RenderContext *>* message = (MessageT<RenderContext *>*)msg;

		lightBehaviorPtr->contextPtr = (RenderContext *)message->value;
	}

	void LightController::OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		LightController* lightBehaviorPtr = (LightController*)pInvokingBehavior;
		MessageT<FrustumHull*>* message = (MessageT<FrustumHull*>*)msg;

		if( !lightBehaviorPtr->lightPtr || !lightBehaviorPtr->contextPtr)
			return;

		Aabb tempAabb = lightBehaviorPtr->gameObject->GetAabb();
		if( message->value->Collides( &tempAabb ) )
		{
			//// Maintain scale of the light geometry
			//D3DXMATRIX *lightMat = lightBehaviorPtr->lightPtr->GetWorldMatrixPtr();
			//Float3 scale(lightMat->_11, lightMat->_22, lightMat->_33);
			//Float4x4 newMat(lightBehaviorPtr->gameObject->GetWorldTransform());
			//newMat.scale(scale);
			//*lightBehaviorPtr->lightPtr->GetWorldMatrixPtr() = *(D3DXMATRIX *)&newMat;

			//lightBehaviorPtr->lightPtr->AddToContextSet();
		}
	}

	void LightController::OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		//LightController* lightBehaviorPtr = (LightController*)pInvokingBehavior;
		//MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

		//// Determine the size of the aabb, including scaling of the mesh to be used for rendering
		//Aabb localAabb = 
		//	lightBehaviorPtr->lightPtr->GetRenderModel()->GetShapes()[0]->GetRenderMesh()->GetAabb();
		//D3DXMATRIX *lightMat = lightBehaviorPtr->lightPtr->GetWorldMatrixPtr();
		//Float3 scale(lightMat->_11, lightMat->_22, lightMat->_33);
		//localAabb.max = localAabb.max * (scale * 0.5f);
		//localAabb.min = localAabb.min * (scale * 0.5f);

		//for(unsigned int i = 0; i < 3; ++i)
		//{
		//	if( message->value->min.v[i] > localAabb.min.v[i] )
		//		message->value->min.v[i] = localAabb.min.v[i];
		//	if( message->value->max.v[i] < localAabb.max.v[i] )
		//		message->value->max.v[i] = localAabb.max.v[i];
		//}
	}

}
