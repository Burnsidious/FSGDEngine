#pragma once

#include "IAttribute.h"
#include "IMessage.h"
//#include "GameObject.h"

namespace EDGOCA
{
	typedef size_t BehaviorNameKey;

	class IBehavior
	{
		friend class GameObjectTemplate;
		friend class Scene;
		friend class GameObject;

		static bool comparePriority(const IBehavior* lhs, const IBehavior* rhs)
		{
			if( lhs->GetPriority() == rhs->GetPriority() )
				return lhs->GetBehaviorNameKey() < rhs->GetBehaviorNameKey();

			return (lhs->GetPriority() < rhs->GetPriority());
		}

	protected:

		GameObject* gameObject;

		IBehavior(void) : gameObject(0) {}

		IAttribute* GetAttribute(AttributeNameKey attributeNameKey);

	public:
		
		enum BehaviorPriority{ PRIORITY_NORMAL = 0x7FFFFFFF };

		virtual ~IBehavior(void) {}

		virtual const AttributeNameKey* GetRequiredAttributes(void) { return 0; }

		virtual const MessageMap& GetMessageMap(void) = 0;

		virtual void OnMessage( IMessage* msg ) = 0;

		virtual void OnAdd(void){}
		virtual void OnRemove(void){}

		virtual unsigned int GetPriority(void)const{ return PRIORITY_NORMAL; }

		virtual BehaviorNameKey GetBehaviorNameKey(void)const = 0;
	};
}