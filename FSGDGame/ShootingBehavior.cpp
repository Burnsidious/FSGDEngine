#include "ShootingBehavior.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;



namespace ShootingMessages
{
	DeclareMessage( MSG_FIRE);
}

namespace
{
	DeclareAttribute(std::string, PROJECTILE_ATTRIBUTE);
}

namespace Behaviors
{
	const AttributeNameKey* ShootingBehavior::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			PROJECTILE_ATTRIBUTE,
			0
		};

		return attribs;
	}

	void ShootingBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( ShootingMessages::MSG_FIRE, OnFire ) );
	}

	void ShootingBehavior::OnFire(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		ShootingBehavior *behaviorPtr = (ShootingBehavior*)pInvokingBehavior;

		Attribute<std::string> *projectileName = 
			(Attribute<std::string> *)behaviorPtr->GetAttribute(PROJECTILE_ATTRIBUTE);

		behaviorPtr->gameObject->SpawnSubObject(projectileName->value.c_str(),
			&behaviorPtr->gameObject->GetWorldTransform());
	}
};