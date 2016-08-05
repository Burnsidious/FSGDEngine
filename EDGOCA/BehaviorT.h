#pragma once

#include "IBehavior.h"
#include "IMessage.h"

namespace EDGOCA
{
	template<typename Behavior>
	class BehaviorT : public IBehavior
	{
		friend class ComponentSystem;

		static unsigned int numReferences;
		
		static MessageMap* messageMap;

		static BehaviorNameKey behaviorNameKey;

		static void SetBehaviorNameKey(BehaviorNameKey nameKey){ behaviorNameKey = nameKey; }

	protected:

		BehaviorT(void)
		{
			if( numReferences == 0 )
			{
				messageMap = new MessageMap();
				Behavior::PopulateMessageMap();
			}

			++numReferences;			
		}

		static void AddMessageHandler(MessageHandler &hndlr)
		{
			messageMap->insert(hndlr);
		}

		static void AddMessageHandler( MessageNameKey messageNameKey, OnMessageProc onMessageProc )
		{
			messageMap->insert( MessageHandler( messageNameKey, onMessageProc ) );
		}

	public:

		virtual ~BehaviorT()
		{
			--numReferences;
			if( numReferences == 0 )
			{
				delete messageMap;
				messageMap = 0;
			}
		}

		const MessageMap& GetMessageMap(void)
		{
			return *messageMap;
		}

		void OnMessage( IMessage *msg )
		{
			MessageMap::iterator msgIter = messageMap->find( msg->GetNameKey() );

			if( msgIter != messageMap->end() )
				msgIter->second( (IBehavior*)this, msg );
		}

		BehaviorNameKey GetBehaviorNameKey(void)const
		{
			return behaviorNameKey;
		}
	};

	template<typename Behavior>
	MessageMap *BehaviorT<Behavior>::messageMap = 0;

	template<typename Behavior>
	unsigned int BehaviorT<Behavior>::numReferences = 0;

	template<typename Behavior>
	BehaviorNameKey BehaviorT<Behavior>::behaviorNameKey = 0;
}