#pragma once

#include "Object.h"
#include <assert.h>

namespace EDGameCore
{
	class GameObject;
	class Message;

	class Animation;
	class Transform;
	class RigidBody;
	class IRenderer;
	class ILight;
	class ICollider;
	class Camera;

	class IComponent : public Object
	{
	public:
		enum ComponentType{ UNKNOWN = -4, GAME_OBJECT = -3, BEHAVIOR = -2, SKY_BOX = -1, ANIMATION, CAMERA, COLLIDER, RENDERER, RIGID_BODY, LIGHT, TRANSFORM, COMPONENT_COUNT };

		IComponent(void){}
		IComponent(const IComponent&){}

		virtual ~IComponent(){}

		virtual GameObject* GetGameObject(void) = 0;
		virtual const GameObject* GetGameObject(void)const = 0;

		virtual Transform* GetTransform(void) = 0;
		virtual IRenderer* GetRenderer(void) = 0;
		virtual RigidBody* GetRigidBody(void) = 0;
		virtual ILight* GetLight(void) = 0;
		virtual ICollider* GetCollider(void) = 0;
		virtual Camera* GetCamera(void) = 0;
		virtual Animation* GetAnimation(void) = 0;
		virtual ComponentType GetComponentType(void)const = 0;
		virtual bool isEnabledInHierarchy(void)const = 0;

		virtual const Transform* GetTransform(void)const  = 0;
		virtual const IRenderer* GetRenderer(void)const  = 0;
		virtual const RigidBody* GetRigidBody(void)const  = 0;
		virtual const ILight* GetLight(void)const  = 0;
		virtual const ICollider* GetCollider(void)const  = 0;
		virtual const Camera* GetCamera(void)const  = 0;
		virtual const Animation* GetAnimation(void)const  = 0;
	};

	class Component : public IComponent
	{
		friend class Game;
		friend class GameObject;
	public:
		Component(void) : gameObject(0) {} 
		Component(const Component&) : gameObject(0) {}

		virtual ~Component();//{}

		const char* GetName(void);
		GameObject* GetGameObject(void){ return gameObject; }
		const GameObject* GetGameObject(void)const{ return gameObject; }
		Transform* GetTransform(void);
		IRenderer* GetRenderer(void);
		RigidBody* GetRigidBody(void);
		ILight* GetLight(void);
		ICollider* GetCollider(void);
		Camera* GetCamera(void);
		Animation* GetAnimation(void);

		const Transform* GetTransform(void)const;
		const IRenderer* GetRenderer(void)const;
		const RigidBody* GetRigidBody(void)const;
		const ILight* GetLight(void)const;
		const ICollider* GetCollider(void)const;
		const Camera* GetCamera(void)const;
		const Animation* GetAnimation(void)const;

		bool isEnabledInHierarchy(void)const;
	private:
		GameObject* gameObject;
	};

	template<typename Derived>
	class ComponentT : public Component
	{
		friend class Game;
		friend class Registry;
	public:
		virtual ~ComponentT(){}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }

	private:

		static RegistrationId typeId;
		static string typeName;
	};

	template<typename Derived>
	RegistrationId ComponentT<Derived>::typeId = 0;
	
	template<typename Derived>
	string ComponentT<Derived>::typeName;
}