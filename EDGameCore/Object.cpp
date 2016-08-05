#include "Object.h"
#include "Game.h"

namespace EDGameCore
{
	void Object::DontDestroyOnLoad(Object* object)
	{ 
		object->destroyOnLoad = false;
	}

	Object::Object(void)
	{
		destroyed = false;
		enabled = true;
		destroyOnLoad = true;
	}

	Object::Object(const Object& rhs)
	{
		destroyed = false;
		enabled = rhs.enabled;
	}

	Object::~Object()
	{
	}

	void Object::enable(void)
	{
		if( isDestroying() )
			return;

		enabled = true;

		OnEnable();
	}

	void Object::disable(void)
	{
		if( enabled == false )
			return;

		enabled = false;

		OnDisable();
	}

	void Object::_OnDestroy(void)
	{
		if( destroyed )
			return;

		destroyed = true;
		disable();

		OnDestroy();
	}
}