#pragma once

#include "Component.h"
#include "GameObject.h"

namespace EDGameCore
{
	class ISkyBox : public Component
	{
	public:
		virtual ~ISkyBox(){}
		virtual void Render(void) = 0;

		ComponentType GetComponentType(void)const{ return SKY_BOX; }
	};

	template<typename Derived>
	class SkyBoxT : public ISkyBox
	{
		friend class Game;
		friend class Registry;

	public:
		virtual ~SkyBoxT(){}

		void* GetDerivedTypePtr(void)
		{ 
			return (Derived*)this;
		}
		
		void Render(void){}
		void LoadState(TiXmlElement* xmlElement){}

		ISkyBox* clone(void)const
		{ 
			return new Derived(*(Derived*)this);
		}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }
	private:

		static RegistrationId typeId;
		static string typeName;

	};

	template<typename Derived>
	RegistrationId SkyBoxT<Derived>::typeId = 0;
	
	template<typename Derived>
	string SkyBoxT<Derived>::typeName;
}