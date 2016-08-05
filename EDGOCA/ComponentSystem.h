#pragma once

#include "StringTable.h"
#include "Attribute.h"
#include "IBehavior.h"
#include "IMessage.h"

namespace EDGOCA
{
	class IAttribute;

	typedef IAttribute* (*AttributeCreator)(AttributeNameKey);
	typedef IBehavior* (*BehaviorCreator)(BehaviorNameKey);

	class ComponentSystem
	{
		StringTable attributeTypeStringTable;
		StringTable attributeNameStringTable;

		std::map< AttributeTypeKey, AttributeCreator > attributeCreatorMap;
		std::map< AttributeNameKey, AttributeTypeKey > attributeNameTypeMap;

		StringTable behaviorNameStringTable;
		std::map< BehaviorNameKey, BehaviorCreator > behaviorCreatorMap;

		StringTable messageNameStringTable;

		ComponentSystem(void){}
		ComponentSystem(const ComponentSystem&){}
		ComponentSystem& operator=(const ComponentSystem&){ return *this; }

		static ComponentSystem* GetInstance(void);

		template<typename type>
		static IAttribute* CreateAttributeFunc(AttributeNameKey nameKey);

		template<typename type>
		static IBehavior* CreateBehaviorFunc(BehaviorNameKey nameKey);

		static void RegisterAttributeCreator( AttributeTypeKey typeKey, AttributeCreator creator );
		static void RegisterBehaviorCreator( BehaviorNameKey nameKey, BehaviorCreator creator );

	public:

		template<typename type>
		static AttributeTypeKey RegisterAttributeType( const char* typeName );

		static AttributeNameKey RegisterAttribute( AttributeTypeKey typeKey, const char* attributeName );

		static IAttribute* CreateAttribute(AttributeNameKey nameKey);

		template<typename type>
		static BehaviorNameKey RegisterBehavior( const char* behaviorName );

		static IBehavior* CreateBehavior(BehaviorNameKey nameKey);

		static MessageNameKey RegisterMessage( const char* messageName );

		static MessageNameKey GetMessageKey(const char *str);

		static AttributeNameKey GetAttributeNameKey( const char* attributeName );
		static MessageNameKey GetMessageNameKey( const char* messageName );
		static BehaviorNameKey GetBehaviorNameKey( const char* behaviorName );
	};

	template<typename type>
	IAttribute* ComponentSystem::CreateAttributeFunc(AttributeNameKey nameKey)
	{
		return new Attribute<type>(nameKey);
	}

	template<typename type>
	IBehavior* ComponentSystem::CreateBehaviorFunc(BehaviorNameKey nameKey)
	{
		return new type();
	}

	template<typename type>
	AttributeTypeKey ComponentSystem::RegisterAttributeType( const char* typeName )
	{
		size_t typeKey = GetInstance()->attributeTypeStringTable.AddString( typeName );

		RegisterAttributeCreator( typeKey, CreateAttributeFunc<type> );

		return typeKey;
	}

	template<typename type>
	BehaviorNameKey ComponentSystem::RegisterBehavior( const char* behaviorName )
	{
		size_t nameKey = GetInstance()->behaviorNameStringTable.AddString( behaviorName );

		RegisterBehaviorCreator( nameKey, CreateBehaviorFunc<type> );
		type::SetBehaviorNameKey( nameKey );

		return nameKey;
	}

#define DeclareMessage(name) const MessageNameKey name = ComponentSystem::RegisterMessage( #name )
#define DeclareAttribute(type, name) const AttributeNameKey name = ComponentSystem::RegisterAttribute( ComponentSystem::RegisterAttributeType<type>( #type ), #name )
#define DeclareBehavior(type, name) const BehaviorNameKey name = ComponentSystem::RegisterBehavior< type >( #type )
};