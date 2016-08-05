#include "SkyBoxBehavior.h"
#include "RenderController.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/DebugRenderer.h"

using namespace EDGOCA;
using namespace EDRendererD3D;
using namespace EDMath;
using namespace EDCollision;

namespace SkyBoxBehaviorMessages
{
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
}

namespace Behaviors
{
	void SkyBoxBehavior::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( SkyBoxBehaviorMessages::MSG_BUILD_LOCAL_AABB, 
			OnBuildLocalAabb ) );
	}

	void SkyBoxBehavior::OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		SkyBoxBehavior *skyBoxBehPtr = (SkyBoxBehavior*)pInvokingBehavior;
		MessageT<Aabb*> *message = (MessageT<Aabb*>*)msg;

		// HACK : HACKZILLA!!! Just making the aabb for the skybox large so that it is always rendered, 
		// even though the skybox is actually a unit cube.  This should be based off the size of the 
		// world instead of hard coded.
		Aabb localAabb(Float3(-1000, -1000, -1000), Float3(1000, 1000, 1000));

		//const Aabb& localAabb = skyBoxBehPtr->m_RenderShape->GetRenderMesh()->GetAabb();
		for(unsigned int i = 0; i < 3; ++i)
		{
			if( message->value->min.v[i] > localAabb.min.v[i] )
				message->value->min.v[i] = localAabb.min.v[i];
			if( message->value->max.v[i] < localAabb.max.v[i] )
				message->value->max.v[i] = localAabb.max.v[i];
		}
	}
}