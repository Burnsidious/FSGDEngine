#include "Renderable.h"
#include "AttributeReaders.h"
#include "RenderController.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/DebugRenderer.h"

using namespace EDGOCA;
using namespace EDRendererD3D;
using std::string;
using std::pair;

namespace RenderableMessages
{
	DeclareMessage( MSG_SET_RENDERSHAPE );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

namespace RenderableAttributes
{
	DeclareAttribute( string, RENDERABLE_PATH_ATTRIB  );
}

namespace Behaviors
{
	void Renderable::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( RenderableMessages::MSG_SET_RENDERSHAPE, OnSetRenderShape ) );
		AddMessageHandler( MessageHandler( RenderableMessages::MSG_FRUSTUM_TEST, OnFrustumTest ) );
		AddMessageHandler( MessageHandler( RenderableMessages::MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
		AddMessageHandler( MessageHandler( RenderableMessages::MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount ) );
		AddMessageHandler( GameObjectMessages::MSG_ATTRIBUTE_CHANGED, OnAttributeChanged );		
	}

	const AttributeNameKey* Renderable::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			RenderableAttributes::RENDERABLE_PATH_ATTRIB,
			0
		};

		return attribs;		
	}

	void Renderable::OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		Renderable* pThis = (Renderable*)pInvokingBehavior;
		MessageT<AttributeNameKey>* message = (MessageT<AttributeNameKey>*)msg;

		if( message->value == RenderableAttributes::RENDERABLE_PATH_ATTRIB )
		{
			Attribute<string>* renderablePathAttrib = (Attribute<string>*)pThis->GetAttribute( 
				message->value );
			MessageT< pair<BehaviorNameKey, string> > setRenderShapeMsg( 
				RenderableMessages::MSG_SET_RENDERSHAPE, pair<BehaviorNameKey, string>( 
				pThis->GetBehaviorNameKey(), renderablePathAttrib->value ) );
			pThis->gameObject->OnMessage( &setRenderShapeMsg );
			
			return;
		}
	}
}