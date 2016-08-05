
#pragma once
#include <string>
#include <map>
#include <list>
using namespace std;

class TiXmlElement;

namespace EDGameCore
{
	typedef unsigned int RegistrationId;
	
	// Object
	//
	// Serves as the base class for all components and derived types.
	// Provides an interface to common functionality required for all types.
	class Object
	{
		friend class Registry;
		friend class Game;

	public:
		Object(void);
		virtual ~Object();
		Object(const Object& rhs);
		
		// Returns the registered type id of the derived class
		virtual RegistrationId GetTypeId(void){ return 0; };

		// Returns the type name of the derived class
		virtual const char* GetTypeName(void) = 0;
	
		// Returns the name of the object
		virtual const char* GetName(void){ return 0; }

		// Has the object been queued for deletion?
		bool isDestroying(void)const{ return destroyed; }

		// Enable the object
		// Invokes OnEnable
		void enable(void);

		// Disable the object
		// Invokes OnDisable
		void disable(void);

		// Is the object enabled?
		bool isEnabled(void)const { return enabled; }

		// Load the state of the object from XML
		virtual void LoadState(TiXmlElement* xmlElement){}

		// Clone the object, copying it's type and internal values
		virtual Object* clone(void)const = 0;

		static void DontDestroyOnLoad(Object* object);

		bool DestroysOnLoad(void)const{ return destroyOnLoad; }

	protected:

		// What to do when the object is enabled
		virtual void OnEnable(void){}

		// What to do when the object is disabled
		virtual void OnDisable(void){}

	private:
		// Destroyed state
		bool destroyed;
		// Enabled state
		bool enabled;

		bool destroyOnLoad;

		// Invoked externally when the object is queued for destruction.
		// Invokes OnDestroy
		void _OnDestroy(void);

		// Involed internally when the object is queued for destruction.
		// Specifies what to do, if anything.
		virtual void OnDestroy(void){}
	};
}