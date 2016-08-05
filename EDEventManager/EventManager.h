#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <functional>

namespace EDEventManager
{
	enum EventFlag
	{
		/// No flag set/default flag
		EM_FLAG_NONE = 0,
		/// Event will be posted ever update
		EM_FLAG_PERSISTENT = 1,
		/// Event will be sent to all listeners immediately
		EM_FLAG_IMMEDIATE = 2,
	};

	enum EventPriority
	{
		/// Event will have the lowest priority
		EM_PRIORITY_LOWEST = 0,
		/// Event will have a low priority
		EM_PRIORITY_LOW,
		/// Event will be processed after other events
		EM_PRIORITY_BELOW_NORMAL,
		/// Events will be processed with a normal priority
		EM_PRIORITY_NORMAL,
		/// Event will be processed before other events
		EM_PRIORITY_ABOVE_NORMAL,
		/// Event will have a high priority
		EM_PRIORITY_HIGH,
		/// Event will have the highest priority
		EM_PRIORITY_HIGHEST
	};

	/// \param eventId - ID of the event.
	/// \param eventData - User supplied data.
	/// \param listener - Object that registered as a listener for this event.
	typedef void (*EM_CALLBACK)(unsigned int eventId, const void *eventData, void *listener);

	/// Wrapper to store listener and callback function
	struct Listener
	{
		/// Constructs a new PriorityEvent
		/// \param listener - Object registered as the listener
		/// \param callback - Callback to call whenever the event is raised
		Listener(void *listener, EM_CALLBACK callback):
			listener(listener),
			callback(callback) 
		{
		}

		/// Object listening for an event
		void *listener;
		/// Callback to call when event is posted
		EM_CALLBACK callback;
	};

	/// Container for priority events
	struct PriorityEvent
	{
		/// Constructs a new PriorityEvent
		/// \param eventId - Event ID
		/// \param eventData - Event data
		/// \param priority - Priority of this event. Lower value priority will be sent after
		///    higher priorities
		/// \param flags - Event flags
		PriorityEvent(unsigned int eventId, const void *eventData = nullptr, 
			EventPriority priority = EM_PRIORITY_NORMAL, EventFlag flags = EM_FLAG_NONE): 
			eventId(eventId),
			eventData(eventData),
			priority(priority),
			flags(flags) 
		{
		}

		/// Priority of the event
		EventPriority priority;
		/// ID of the event
		unsigned int eventId;
		/// Event flags
		EventFlag flags;
		/// User supplied data for the event
		const void *eventData;

		/// Determins if this event has a greater priority than the right hand side event
		inline bool operator >(const PriorityEvent &r) const
		{
			if(priority == r.priority)
			{
				if(eventId == r.eventId)
				{
					if(eventData == r.eventData)
					{
						return flags > r.flags;
					}
					return eventData > r.eventData;
				}
				return eventId > r.eventId;
			}
			return priority > r.priority;
		}
	};

	/// Primary Event Manager
	/// For event based communication via RegisterForEvent and PostEvent
	class EventManager
	{
	public:
		/// Gets the EventManager instance.
		/// \return The one and only one EventManager
		static EventManager * GetInstance();

		/// Adds a new event to be processed during the next update. Duplicate events will be
		///   discarded.
		/// \param eventName - Name of the event being posted
		/// \param data - User specified data to be attached to the event.
		/// \param priority - Priority of this event. See EM_PRIORIRTY_* enums.
		/// \param flags - Flags for processing this event. See EM_FLAG_* enums.
		void PostEvent(const std::string &eventName, const void *data = nullptr, 
			EventPriority priority = EM_PRIORITY_NORMAL, EventFlag flags = EM_FLAG_NONE);
		
		/// Adds a new event to be processed during the next update. Duplicate events will be
		///   discarded.
		/// \param eventId - Id of the event being posted
		/// \param data - User specified data to be attached to the event.
		/// \param priority - Priority of this event. See EM_PRIORIRTY_* enums
		/// \param flags - Flags for processing this event. See EM_FLAG_* enums.
		void PostEvent(unsigned int eventId, const void *data = nullptr, 
			EventPriority priority = EM_PRIORITY_NORMAL, EventFlag flags = EM_FLAG_NONE);

		/// Sends all posted and persistent events.
		void Update();

		/// Registers the specified component to listen for specified event name. Callback will be
		///   called whenever event is raised.
		/// \param eventName - Name of the event to unregister for
		/// \param listener - Object registering as a listener for this event.
		/// \param callback - Function to call whenever event is raised.
		/// \return Event Id of the eventName.
		unsigned int RegisterForEvent(const std::string &eventName, void *listener, EM_CALLBACK callback);

		/// Registers the specified component to listen for specified event type. Callback will be
		///   called whenever event is raised.
		/// \param eventId - Id of the event to unregister for
		/// \param listener - Object registering as a listener for this event.
		/// \param callback - Function to call whenever event is raised.
		void RegisterForEvent(unsigned int eventId, void *listener, EM_CALLBACK callback);

		/// Removes a listener for a specific event type for a component
		/// \param eventName - Name of the event to unregister
		/// \param listener - Object that registered as a listener of the event.
		void UnRegisterForEvent(const std::string &eventName, void *listener);

		/// Removes a listener for a specific event type for a component
		/// \param eventId - Id of the event to unregister
		/// \param listener - Object that registered as a listener of the event.
		void UnRegisterForEvent(unsigned int eventId, void *listener);

		/// Deletes all persistent events with the specified name.
		/// \param eventId - ID of persistent event to delete
		void DeletePersistentEvent(unsigned int eventId);

		/// Deletes all persistent events with the specified name.
		/// \param eventName - Name of persistent event to delete
		void DeletePersistentEvent(const std::string &eventName);

		/// Deletes a persistent event with the specified id and data.
		/// \param eventName - ID of persistent event to delete
		/// \param eventData - Data persistent event was created with
		void DeletePersistentEvent(const std::string &eventName, void *eventData);

		/// Deletes a persistent event with the specified name and data.
		/// \param eventId - Name of persistent event to delete
		/// \param eventData - Data persistent event was created with
		void DeletePersistentEvent(unsigned int eventId, void *eventData);

		/// Calculates a hash for the specified event name 
		/// \param eventName - Name of the event to get an Id for
		/// \return Id corresponding to the specified event name.
		unsigned int GetEventId(const std::string &eventName);

	private:
		typedef std::unordered_multimap<unsigned int, Listener> ListenerMap;
		typedef std::pair<unsigned int, Listener> ListenerMapPair;
		typedef std::unordered_multimap<unsigned int, PriorityEvent> EventMap;
		typedef std::pair<unsigned int, PriorityEvent> EventMapPair;
		typedef std::set<PriorityEvent, std::greater<PriorityEvent>> EventSet;
		
		// Singleton Interface
		static EventManager *instance;
		EventManager() {};
		EventManager(const EventManager &r);
		EventManager &operator=(const EventManager &r);

		/// Sends specified event to all listeners of this event.
		/// \param eventToPost - Event to send to listeners
		void PostEventToListeners(const PriorityEvent &eventToPost);

		/// Ordered set of priority events. These are the events that need to be sent
		///   during the next update.
		EventSet eventSet;
		
		/// Ordered map of persistent events. These events will be called every update.
		/// ( Key = EventId, Value = PriorityEvent )
		EventMap persistentEventMap;

		/// Map of listeners for each event. Allows multiple listeners per Event.
		/// (Key = EventId, Value = {Callback, Component})
		ListenerMap eventListenerMap;
	};

}