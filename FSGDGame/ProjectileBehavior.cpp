#include "ProjectileBehavior.h"
#include "WorldData.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "Behaviors.h"

using namespace EDGOCA;
using namespace EDMath;

namespace ProjectileMessages
{
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_COLLIDE );
}

namespace Behaviors
{
	void ProjectileBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( ProjectileMessages::MSG_UPDATE, OnUpdate ) );
		AddMessageHandler( MessageHandler( ProjectileMessages::MSG_COLLIDE, OnCollide ) );
	}

	void ProjectileBehavior::OnUpdate( IBehavior* invokingBehavior, IMessage* message )
	{
		ProjectileBehavior *behaviorPtr = (ProjectileBehavior *)invokingBehavior;

		behaviorPtr->gameObject->TranslateLocal(Float3(0, 0, 1) * behaviorPtr->ComputeSpeed());
	}

	void ProjectileBehavior::OnCollide( IBehavior* invokingBehavior, IMessage* message )
	{
		ProjectileBehavior* behaviorPtr = (ProjectileBehavior*)invokingBehavior;

		MessageT<GameObject*>* msg = (MessageT<GameObject*>*)message;

		GameObject *projTopParentPtr = behaviorPtr->gameObject->GetOwner()->GetRootObject();
		GameObject *colliderTopParentPtr = msg->value->GetRootObject();

		if(colliderTopParentPtr == projTopParentPtr)
			return;
		else 
		{
			if(colliderTopParentPtr->GetGameObjectFlags() & (1 << GameObject::STATIC))
				return;
			// Hack projectile death
			//colliderTopParentPtr = nullptr;
			//behaviorPtr->gameObject->RemoveBehavior(BHVR_PROJECTILE);
			//behaviorPtr->baseSpeed = 0;
		}

	}

	float ProjectileBehavior::ComputeSpeed()
	{
		return baseSpeed * WorldData::GetDeltaTime();
	}
}
