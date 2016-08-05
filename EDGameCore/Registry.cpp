#include "Registry.h"
#include <functional>
#include <assert.h>
#include "Game.h"

#undef RegisterType

namespace EDGameCore
{
	map<RegistrationId, string> Registry::stringMap;
	list<Object*> Registry::destroyingList;

	map< RegistrationId, ObjectCreator > Registry::creatorMap;

	RegistrationId Registry::RegisterString( const char* identifier )
	{
		RegistrationId hash = (unsigned int)std::hash<string>()(identifier);

		auto stringIter = stringMap.find(hash);

		if( stringIter == stringMap.end() )
			stringMap.insert( pair<RegistrationId, string>(hash, identifier) ).first;
		else
		{
			// Hash collision! Oh noes! Time to use a better method (or a different identifier)
			assert( strcmp( identifier, stringIter->second.c_str() ) == 0 );
		}
		
		return hash;		
	}

	RegistrationId Registry::RegisterType(const char* identifier, ObjectCreator creator)
	{
		RegistrationId hash = RegisterString(identifier);
		
		creatorMap.insert( make_pair(hash, creator) );

		return hash;
	}

	RegistrationId Registry::GetStringRegistrationId( const char* identifier )
	{
		RegistrationId hash = (unsigned int)std::hash<string>()(identifier);

		auto stringIter = stringMap.find(hash);

		if( stringIter == stringMap.end() )
			return 0;

		return hash;
	}

	Object* Registry::create(RegistrationId id)
	{
		auto creatorIter = creatorMap.find(id);
		if( creatorIter == creatorMap.end() )
			return 0;

		return creatorIter->second();
	}

	Object* Registry::create(const char* identifier)
	{
		return create( GetStringRegistrationId(identifier) );
	}

	void Registry::destroyImmediate(Object* object)
	{
		delete object;
	}

	void Registry::destroy(Object* object)
	{
		if( !object->isDestroying() )
		{
			object->_OnDestroy();

			destroyingList.push_back(object);
		}
	}

	void Registry::CleanUp(void)
	{
		while( !destroyingList.empty() )
		{
			destroyImmediate( destroyingList.front() );
			destroyingList.pop_front();
		}
	}
}