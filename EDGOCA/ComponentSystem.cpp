#include "precompiled.h"
#include "ComponentSystem.h"

using namespace std;

namespace EDGOCA
{
	ComponentSystem* ComponentSystem::GetInstance(void)
	{
		static ComponentSystem instance;
		return &instance;
	}

	void ComponentSystem::RegisterAttributeCreator( AttributeTypeKey typeKey, AttributeCreator creator )
	{
		GetInstance()->attributeCreatorMap.insert( pair<AttributeTypeKey, AttributeCreator>( typeKey, creator ) );
	}

	void ComponentSystem::RegisterBehaviorCreator( BehaviorNameKey nameKey, BehaviorCreator creator )
	{
		GetInstance()->behaviorCreatorMap.insert( pair<BehaviorNameKey, BehaviorCreator>(nameKey, creator) );
	}

	AttributeNameKey ComponentSystem::RegisterAttribute( AttributeTypeKey typeKey, const char* attributeName )
	{
		size_t nameKey = GetInstance()->attributeNameStringTable.AddString( attributeName );

		GetInstance()->attributeNameTypeMap.insert( pair<AttributeNameKey, AttributeTypeKey>( nameKey, typeKey ) );

		return nameKey;
	}

	IAttribute* ComponentSystem::CreateAttribute(AttributeNameKey nameKey)
	{
		map< AttributeNameKey, AttributeTypeKey >::iterator typeIter = GetInstance()->attributeNameTypeMap.find( nameKey );

		if( typeIter == GetInstance()->attributeNameTypeMap.end() )
			return 0;

		map< AttributeTypeKey, AttributeCreator >::iterator creatIter = GetInstance()->attributeCreatorMap.find( typeIter->second );

		if( creatIter == GetInstance()->attributeCreatorMap.end() )
			return 0;

		return creatIter->second(nameKey);
	}

	IBehavior* ComponentSystem::CreateBehavior(BehaviorNameKey nameKey)
	{
		map< BehaviorNameKey, BehaviorCreator >::iterator creatIter = GetInstance()->behaviorCreatorMap.find( nameKey );

		if( creatIter == GetInstance()->behaviorCreatorMap.end() )
			return 0;

		return creatIter->second(nameKey);
	}

	MessageNameKey ComponentSystem::RegisterMessage( const char* messageName )
	{
		return GetInstance()->messageNameStringTable.AddString( messageName );
	}

	MessageNameKey ComponentSystem::GetMessageKey(const char *str)
	{
		return GetInstance()->messageNameStringTable.GetStringKey(str);
	}

	AttributeNameKey ComponentSystem::GetAttributeNameKey( const char* attributeName )
	{
		return GetInstance()->attributeNameStringTable.GetStringKey( attributeName );
	}

	MessageNameKey ComponentSystem::GetMessageNameKey( const char* messageName )
	{
		return GetInstance()->messageNameStringTable.GetStringKey( messageName );
	}

	BehaviorNameKey ComponentSystem::GetBehaviorNameKey( const char* behaviorName )
	{
		return GetInstance()->behaviorNameStringTable.GetStringKey( behaviorName );
	}
}