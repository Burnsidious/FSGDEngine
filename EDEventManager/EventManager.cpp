#include "EventManager.h"

#include "../EDUtilities/Hasher.h"

#ifdef _DEBUG
#include <map>
#include <algorithm>
#endif
#include <functional>
//#include <utility>
#include <assert.h>

namespace EDEventManager
{
	EventManager * EventManager::instance = nullptr;

	EventManager * EventManager::GetInstance()
	{
		// Some destructors were calling GetInstance() even after eventmanager was destroyed,
		//   causing another copy to be made. Static local is simpler anyways.
		static EventManager theEventManager;
		return &theEventManager;
	}

	void EventManager::PostEvent(const std::string &eventName, const void *data, EventPriority priority, EventFlag flags)
	{
		PostEvent(GetEventId(eventName), data, priority, flags);
	}
	
	void EventManager::PostEvent(unsigned int eventId, const void *data, EventPriority priority, EventFlag flags)
	{
		if(flags & EM_FLAG_IMMEDIATE)
		{
			// Instantly send this event
			PostEventToListeners(PriorityEvent(eventId, data, priority, flags));
		}
		else if(flags & EM_FLAG_PERSISTENT)
		{
			// Add to persistent events
			persistentEventMap.insert(EventMapPair(eventId, PriorityEvent(eventId, data, priority, flags)));
		}
		else
		{
			// Add event to be sent out on the next update
			eventSet.insert(PriorityEvent(eventId, data, priority, flags));
		}
	}

	void EventManager::Update()
	{
		// Insert persistent events into eventset 
		for(auto eventIter = persistentEventMap.begin(); eventIter != persistentEventMap.end(); ++eventIter)
		{
			eventSet.insert(eventIter->second);
		}

		// Process scheduled events
		for(auto eventIter = eventSet.begin(); eventIter != eventSet.end(); ++eventIter)
		{
			PostEventToListeners(*eventIter);
		}

		eventSet.clear();
	}

	unsigned int EventManager::RegisterForEvent(const std::string &eventName, void *listener, 
		EM_CALLBACK callback)
	{
		unsigned int id = GetEventId(eventName);
		eventListenerMap.insert(ListenerMapPair(id,Listener(listener,callback)));
		return id;
	}

	void EventManager::RegisterForEvent(unsigned int eventId, void *listener, EM_CALLBACK callback)
	{
		eventListenerMap.insert(ListenerMapPair(eventId,Listener(listener,callback)));
	}

	void EventManager::UnRegisterForEvent(const std::string &eventName, void *listener)
	{
		unsigned int id = GetEventId(eventName);
		UnRegisterForEvent(id, listener);
	}

	void EventManager::UnRegisterForEvent(unsigned int eventId, void *listener)
	{
		auto range = eventListenerMap.equal_range(eventId);
		for(auto iter = range.first; iter != range.second; ++iter)
		{
			if ((*iter).second.listener == listener)
			{
				eventListenerMap.erase(iter);
				break;
			}
		}
	}

	void EventManager::DeletePersistentEvent(unsigned int eventId, void *eventData)
	{
		auto eventsFound = persistentEventMap.equal_range(eventId);

		for(auto eventIter = eventsFound.first; eventIter != eventsFound.second; ++eventIter)
		{
			if((*eventIter).second.eventData == eventData)
			{
				persistentEventMap.erase(eventIter);
				return;
			}
		}
	}

	void EventManager::DeletePersistentEvent(const std::string &eventName, void *eventData)
	{
		unsigned int id = GetEventId(eventName);
		DeletePersistentEvent(id, eventData);
	}

	void EventManager::DeletePersistentEvent(unsigned int eventId)
	{
		persistentEventMap.erase(eventId);
	}

	void EventManager::DeletePersistentEvent(const std::string &eventName)
	{
		unsigned int id = GetEventId(eventName);
		DeletePersistentEvent(id);
	}

	unsigned int EventManager::GetEventId(const std::string &eventName)
	{
		return EDUtilities::Hasher::HashString(eventName);
	}

	void EventManager::PostEventToListeners(const PriorityEvent &eventToPost)
	{
		auto eventListeners = eventListenerMap.equal_range(eventToPost.eventId);
		for(auto eventListenerIter = eventListeners.first; eventListenerIter != eventListeners.second; ++eventListenerIter)
		{
			Listener &currentListener = (*eventListenerIter).second;

			currentListener.callback(eventToPost.eventId, eventToPost.eventData, (*eventListenerIter).second.listener);
		}
	}
}