#pragma once

#include "Object.h"
#include "Component.h"
#include "Behavior.h"
#include "Collider.h"
#include "../EDCollision/EDCollision.h"
#include <set>
#include <map>
#include <unordered_map>

using namespace std;

namespace EDGameCore
{
	class Message;
	class IBehavior;
	class IRenderer;
	class RigidBody;
	class Transform;
	class ILight;
	class Camera;
	class Animation;

	class GameObject : public IComponent
	{
		friend class Game;
		friend class Registry;
		friend class ICollider;
		friend class Physics;

		template<typename Derived>
		friend class Behavior;

	public:
		GameObject(void);
		GameObject(const GameObject& rhs);

		~GameObject();

		ComponentType GetComponentType(void)const{ return GAME_OBJECT; }

		void OnMessage(Message& message);
		void OnMessage(Message& message, const void * data);
		void BroadcastMessage(Message& message);
		void SendMessageUp(Message& message);
		
		template<typename T>
		T* AddBehavior(void)
		{
			IBehavior* result = AddBehavior( EDGameCore::GetTypeId<T>() );
			
			if( result == 0 )
				return 0;

			return (T*)result;
		}

		IBehavior* AddBehavior(const char* identifier);
		IBehavior* AddBehavior(IBehavior* b);

		IRenderer* SetRenderer(IRenderer* r);
		ICollider* SetCollider(ICollider* c);
		RigidBody* SetRigidBody(RigidBody* r);
		ILight* SetLight(ILight* l);
		Camera* SetCamera(Camera* c);
		Animation* SetAnimation(Animation* a);

		const char* GetName(void)const{ return name.empty() ? 0 : name.c_str(); }
		RegistrationId GetNameHash(void)const{ return nameHash; }

		GameObject* GetGameObject(void){ return this; }
		const GameObject* GetGameObject(void)const{ return this; }

		IRenderer* GetRenderer(void){ return renderer; }
		ICollider* GetCollider(void){ return collider; }
		Transform* GetTransform(void){ return transform; }
		RigidBody* GetRigidBody(void){ return rigidBody; }
		ILight* GetLight(void){ return light; }
		Camera* GetCamera(void){ return camera; }
		Animation* GetAnimation(void){ return animation; }
		
		const IRenderer* GetRenderer(void)const { return renderer; }
		const ICollider* GetCollider(void)const { return collider; }
		const Transform* GetTransform(void)const { return transform; }
		const RigidBody* GetRigidBody(void)const { return rigidBody; }
		const ILight* GetLight(void)const { return light; }
		const Camera* GetCamera(void)const { return camera; }
		const Animation* GetAnimation(void)const { return animation; }
		const GameObject* GetParent(void)const;

		static GameObject* Find(const char* name);

		unsigned int GetInstanceId(void){ return instanceId; }

		static GameObject* GetGameObjectInstance(unsigned int instanceId);

		void Update(void);

		void FixedUpdate(void);

		void LateUpdate(void);

		void SetName(const char* n);

		void LoadState(TiXmlElement* xmlElement);

		unsigned int GetLayer(void)const{ return layer; }
		unsigned int GetLayerMask(void)const;

		void SetLayer(const char* identifier);
		void SetLayer(unsigned int l){ layer = l; }

		void AddTag(const char* identifier);
		void AddTag(RegistrationId id);

		bool HasTag(const char* identifier);
		bool HasTag(RegistrationId id);

		void RemoveTag(const char* identifier);
		void RemoveTag(RegistrationId id);

		bool isEnabledInHierarchy(void)const;

		bool HasBehavior(const char* identifier);
		bool HasBehavior(RegistrationId id);

		template<typename Func>
		static void ForEachGameObject(Func func)
		{
			for_each( gameObjectList.begin(), gameObjectList.end(), func );
		}

		GameObject* clone(void)const
		{
			return new GameObject(*this);
		}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }
		
		IBehavior* GetBehavior(const char* identifier);
		IBehavior* GetBehavior(RegistrationId id);
		
		template<typename T>
		T* GetBehavior(void)
		{
			return (T*)GetBehavior( Game::GetTypeId<T>() );
		}

		template<typename Func>
		void ForEachComponent(Func func)
		{
			for(unsigned int i = 0; i < COMPONENT_COUNT; ++i)
			{
				if( components[i] != 0 )
					func( components[i] );
			}
		}

		template<typename Func>
		void ForEachBehavior(Func func)
		{
			for(auto iter = behaviorMap.begin(); iter != behaviorMap.end(); ++iter)
				func(iter->second);
		}

	private:

		IBehavior* AddBehavior(RegistrationId id);
		IRenderer* SetRenderer(RegistrationId id);
		IRenderer* SetRenderer(const char* identifier);
		ICollider* SetCollider(RegistrationId id);
		ICollider* SetCollider(const char* identifier);
		RigidBody* SetRigidBody(RegistrationId id);
		RigidBody* SetRigidBody(const char* identifier);
		ILight* SetLight(RegistrationId id);
		ILight* SetLight(const char* identifier);
		Camera* SetCamera(RegistrationId id);
		Camera* SetCamera(const char* identifier);
		Animation* SetAnimation(RegistrationId id);
		Animation* SetAnimation(const char* identifier);

		void Awake(void);

		void RemoveBehavior(RegistrationId id);

		void OnDestroy(void);

		static GameObject* Load(const char* fileName);
		static GameObject* Load(TiXmlElement* xmlElement);

		void OnEnable(void);
		void OnDisable(void);

		void OnCollisionEnter(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact);
		void OnCollisionStay(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact);
		void OnCollisionExit(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact);

		void OnTriggerEnter(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact);
		void OnTriggerStay(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact);
		void OnTriggerExit(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact);

		unsigned int instanceId;

		map<RegistrationId, IBehavior*> behaviorMap;

		string name;
		RegistrationId nameHash;

		unsigned int layer;

		std::set<RegistrationId> tags;

		union
		{
			struct
			{
				Animation* animation;
				Camera* camera;
				ICollider* collider;
				IRenderer* renderer;
				RigidBody* rigidBody;
				ILight* light;
				Transform* transform;
			};

			Component* components[COMPONENT_COUNT];
		};

		static list<GameObject*> gameObjectList;

		static unordered_map< unsigned int, GameObject* > instanceMap;
		static unsigned int instanceCounter;

		static RegistrationId typeId;
		static string typeName;
	};
}