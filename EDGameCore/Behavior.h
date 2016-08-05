#pragma once

#include "Component.h"
#include "GameObject.h"
#include "../EDCollision/EDCollision.h"

namespace EDGameCore
{
	class Message;

	class IBehavior : public Component
	{
		friend class GameObject;
	public:
		IBehavior(void){}
		virtual ~IBehavior(void){}

		virtual void Update(void){}
		virtual void FixedUpdate(void){}
		virtual void LateUpdate(void){}
		virtual void OnMessage(Message& message){}
		virtual void OnMessage(Message& message, const void * data){}
		virtual RegistrationId* GetRequiredBehaviors(void)const { return 0; }

		virtual void OnCollisionEnter(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact){}
		virtual void OnCollisionStay(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact){}
		virtual void OnCollisionExit(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact){}

		virtual void OnTriggerEnter(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact){}
		virtual void OnTriggerStay(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact){}
		virtual void OnTriggerExit(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact){}

		ComponentType GetComponentType(void)const{ return BEHAVIOR; }

		IBehavior* clone(void)const = 0;

	protected:
		virtual void Awake(void){}
	};

	template<typename Derived>
	class Behavior : public IBehavior
	{
		friend class Game;
		friend class Registry;
	public:
		virtual ~Behavior()
		{
			GameObject* gameObj = GetGameObject();

			if( gameObj != 0 )
				gameObj->RemoveBehavior( GetTypeId() );
		}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }

		void LoadState(TiXmlElement* xmlElement){}

		IBehavior* clone(void)const
		{
			return new Derived(*(Derived*)this);
		}

	private:
		static RegistrationId typeId;
		static string typeName;
	};

	template<typename Derived>
	RegistrationId Behavior<Derived>::typeId = 0;
	
	template<typename Derived>
	string Behavior<Derived>::typeName;
}