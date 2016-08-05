#include "BaseRenderable.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDUtilities/Hasher.h"
#include "RenderController.h"
#include "AnimatedBone.h"

#include "../EDRendererD3D/RenderShapeSkinned.h"

using namespace EDGOCA;
using namespace EDRendererD3D;
using namespace Behaviors;
using namespace GameObjectMessages;
using std::pair;

namespace BaseRenderableMessages
{
	DeclareMessage( MSG_SET_BASE_RENDERSHAPE_LIST );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
	DeclareMessage( MSG_ANIMATION_UPDATE );
}

template<>
void Attribute<RenderShapePathList>::ReadXML(TiXmlElement* valElement)
{
	TiXmlElement* renderShapeEle = valElement->FirstChildElement("RenderShape");
	
	for(; renderShapeEle != 0; renderShapeEle = renderShapeEle->NextSiblingElement("RenderShape") )
	{
		pair< RenderShapeLODPaths, TransformKey > renderShapePaths;
		renderShapePaths.second = -1;

		TiXmlElement* xformNameEle = renderShapeEle->FirstChildElement("TransformName");
		if( xformNameEle != 0 && xformNameEle->GetText() != 0 )
			renderShapePaths.second = static_cast<TransformKey>(
			EDUtilities::Hasher::HashString( xformNameEle->GetText() ));
		
		TiXmlElement* lodPathEle = renderShapeEle->FirstChildElement("Path");
		for(; lodPathEle != 0; lodPathEle = lodPathEle->NextSiblingElement("Path") )
		{
			if( lodPathEle->GetText() != 0 )
				renderShapePaths.first.push_back( lodPathEle->GetText() );
		}

		if( renderShapePaths.first.size() != 0 )
			this->value.push_back( renderShapePaths );
	}
}

namespace
{
	DeclareAttribute( RenderShapePathList, BASE_RENDERABLE_SHAPE_PATH_LIST  );
}

namespace Behaviors
{
	void BaseRenderable::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( BaseRenderableMessages::MSG_SET_BASE_RENDERSHAPE_LIST, OnSetRenderShapeList ) );
		AddMessageHandler( MessageHandler( BaseRenderableMessages::MSG_FRUSTUM_TEST, OnFrustumTest ) );
		AddMessageHandler( MessageHandler( BaseRenderableMessages::MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
		AddMessageHandler( MessageHandler( BaseRenderableMessages::MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount ) );
		AddMessageHandler( MSG_ATTRIBUTE_CHANGED, OnAttributeChanged );		
		AddMessageHandler( MessageHandler( BaseRenderableMessages::MSG_ANIMATION_UPDATE, OnAnimationUpdate ) );		
	}

	const AttributeNameKey* BaseRenderable::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			BASE_RENDERABLE_SHAPE_PATH_LIST,
			0
		};

		return attribs;		
	}

	void BaseRenderable::OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		BaseRenderable* pThis = (BaseRenderable*)pInvokingBehavior;
		MessageT<AttributeNameKey>* message = (MessageT<AttributeNameKey>*)msg;
		
		if( message->value == BASE_RENDERABLE_SHAPE_PATH_LIST )
		{
			Attribute<RenderShapePathList>* renderShapePathList = (Attribute<RenderShapePathList>*)pThis->GetAttribute( message->value );

			MessageT< RenderShapePathList* > setShapeListMessage( BaseRenderableMessages::MSG_SET_BASE_RENDERSHAPE_LIST );
			setShapeListMessage.value = &renderShapePathList->value;
			pThis->gameObject->OnMessage( &setShapeListMessage );
		}
	}

	void BaseRenderable::OnAnimationUpdate( IBehavior *pInvokingBehavior, IMessage *msg )
	{
		BaseRenderable *pThis = (BaseRenderable*)pInvokingBehavior;
		MessageT<std::vector<EDMath::Float4x4> &> *bonesPtr = 
			(MessageT<std::vector<EDMath::Float4x4> &>*)msg;

		RenderShapeSkinned *skinShape = (RenderShapeSkinned *)pThis->renderShapeList.begin()->first;

		skinShape->SetBoneMatrices(bonesPtr->value);
	}

}