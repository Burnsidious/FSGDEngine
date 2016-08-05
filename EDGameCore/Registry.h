#pragma once

#include <map>
#include <string>
#include "Object.h"
using namespace std;

namespace EDGameCore
{
	typedef unsigned int RegistrationId;
	typedef Object* (*ObjectCreator)(void);

	class Registry
	{
	protected:
		Registry(void){}
		Registry(const Registry&){}
		Registry& operator=(const Registry&){return *this;}
		virtual ~Registry(){}

		static void destroyImmediate(Object* object);

		static map<RegistrationId, string> stringMap;

		static map< RegistrationId, ObjectCreator > creatorMap;

		static list<Object*> destroyingList;

		void CleanUp(void);

	public:
		static RegistrationId RegisterString( const char* identifier );

		static RegistrationId GetStringRegistrationId( const char* identifier );

		static Object* create(RegistrationId id);

		static Object* create(const char* identifier);

		template<typename T>
		static T* create(void)
		{
			Object* result = create( GetTypeId<T>() );

			if( result != 0 )
				return (T*)result;

			return 0;
		}

		static void destroy(Object* object);

		template<typename T>
		static RegistrationId GetTypeId(void)
		{
			return T::typeId;
		}

		template<typename T>
		static const char* GetTypeName(void)
		{
			return T::typeName.empty() ? 0 : T::typeName.c_str();
		}

		template<typename T>
		static RegistrationId RegisterTypeT(const char* identifier)
		{
			if( T::typeId != 0 )
				return T::typeId;

			RegistrationId result = Registry::RegisterType(identifier, creator<T>);
			T::typeId = result;
			T::typeName = identifier;

			return result;
		}

		static RegistrationId RegisterType(const char* identifier, ObjectCreator creator);
	};

	template<typename T>
	Object* creator(void){ return new T; }
}