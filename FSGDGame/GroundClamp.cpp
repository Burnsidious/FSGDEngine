#include "GroundClamp.h"
#include "WorldData.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"

using std::pair;
using namespace EDGOCA;
using namespace EDMath;
using namespace EDCollision;

namespace GroundClampMessages
{
	DeclareMessage( MSG_COLLIDE );
	DeclareMessage( MSG_INTERSECT_SEGMENT );
	DeclareMessage( MSG_UPDATE );
}

namespace GroundClampAttributes
{
	DeclareAttribute(bool, GROUNDED_ATTRIB);
}

namespace Behaviors
{
	const AttributeNameKey* GroundClamp::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			GroundClampAttributes::GROUNDED_ATTRIB,
			0
		};

		return attribs;
	}

	void GroundClamp::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( GroundClampMessages::MSG_COLLIDE, OnCollide ) );
		AddMessageHandler( MessageHandler( GroundClampMessages::MSG_UPDATE, OnUpdate ) );
	}

	void GroundClamp::OnCollide( IBehavior* invokingBehavior, IMessage* message )
	{
		GroundClamp* pGroundClamp = (GroundClamp*)invokingBehavior;

		MessageT<GameObject*>* msg = (MessageT<GameObject*>*)message;
		
		MessageT< pair<bool, Segment> > segMsg( GroundClampMessages::MSG_INTERSECT_SEGMENT );
		segMsg.value.first = false;
		segMsg.value.second.start = (pGroundClamp->gameObject->GetAabb().min + pGroundClamp->gameObject->GetAabb().max) * 0.5f;
		segMsg.value.second.start.y = pGroundClamp->gameObject->GetAabb().max.y;
		segMsg.value.second.end = segMsg.value.second.start;
		segMsg.value.second.end.y = pGroundClamp->gameObject->GetAabb().min.y;
		Float3 endPt = segMsg.value.second.end;
		segMsg.value.second.end.y -= 0.125f;

		msg->value->OnMessage( &segMsg );

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)pGroundClamp->GetAttribute(
			GroundClampAttributes::GROUNDED_ATTRIB);
		if( segMsg.value.first == true )
		{
			groundedAttrib->value = true;
			pGroundClamp->gameObject->TranslateGlobal( segMsg.value.second.end - endPt );
		}
	}

	void GroundClamp::OnUpdate( IBehavior* invokingBehavior, IMessage* message )
	{
		GroundClamp* pGroundClamp = (GroundClamp*)invokingBehavior;

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)pGroundClamp->GetAttribute(
			GroundClampAttributes::GROUNDED_ATTRIB);

		float deltaTime = WorldData::GetDeltaTime();

		if( groundedAttrib->value )
			groundedAttrib->value = false;
		// HACK
		// This is a simple hacked gravity simulation
		// This should be removed and replaced with a more robust solution, likely in the Physics behavior
		// Of note, the order that the grounded flag is adjusted and gravity is applied are very important to this hack
		else //if(deltaTime < 1.0f)
		{
			Float3 gravity(0.0f, -9.8f, 0.0f);
			Float3 pos = pGroundClamp->gameObject->GetWorldTransform().WAxis;

			Float3 newPos = pos + gravity * deltaTime;
			Float3 delta = newPos - pos;
			pGroundClamp->gameObject->TranslateGlobal(delta);
		}
	}
}