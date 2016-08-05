#pragma once


#include "TransformNode.h"
#include "IBehavior.h"
//#include "../EDCollision/CollidableTypes.h"
#include "../EDMath/aabb.h"
#include "../EDUtilities/ContentManager.h"

namespace GameObjectMessages
{
	extern const EDGOCA::MessageNameKey MSG_ATTRIBUTE_CHANGED;
}

namespace EDGOCA
{
	typedef int TransformKey;

	class DynAabbTreeLeafNode;

	struct TransformData
	{
		TransformKey transformKey;
		EDMath::Float4x4 localMatrix;
		TransformNode::TransformFlag transformFlag;
	};

	class GameObjectTemplate
	{
		friend class GameObject;
		friend class Scene;

		EDMath::Aabb aabb;

		unsigned int id;
		unsigned int flags;

		std::set< BehaviorNameKey > behaviors;
		std::map< AttributeNameKey, IAttribute* > attributes;
		std::list< TransformData > transforms;
		std::list< std::pair<std::string, TransformKey> > childObjects;

	public:

		static GameObjectTemplate* LoadXML(const char* xmlFilePtr, const char* indentifier);

		const EDMath::Aabb& GetAabb(void) const { return aabb; }

		unsigned int GetID(void){ return id; }

		unsigned int GetFlags(void){ return flags; }

		~GameObjectTemplate()
		{
			std::map< AttributeNameKey, IAttribute* >::iterator iter = attributes.begin();
			std::map< AttributeNameKey, IAttribute* >::iterator iterEnd = attributes.end();
			for(; iter != iterEnd; ++iter)
			{
				delete (*iter).second;
				(*iter).second = 0;
			}
		}
	};

	class GameObject : public TransformNode
	{
		friend class Scene;
		friend class IBehavior;

		EDMath::Aabb aabb;
		EDMath::Aabb localAabb;
		EDUtilities::ContentHandle<GameObjectTemplate> gameObjectTemplateHandle;
		
		unsigned int gameObjectFlags;

		void DirtyNode(void);

		DynAabbTreeLeafNode* dynAabbTreeLeaf;

		DynAabbTreeLeafNode* visLeaf;

		std::map< BehaviorNameKey, IBehavior* > behaviors;
		std::map< AttributeNameKey, std::pair< IAttribute *, unsigned int > > attributes;
		std::map< MessageNameKey, std::set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) > > messageToBehaviorMap;

		std::map< GameObject*, TransformKey > childObjects;
		std::map< TransformKey, TransformNode* > transformSet;

		std::list< GameObject *> subObjects;
		// Pointer to GameObject that spawned this object, if one did
		GameObject *ownerPtr;

		void Update(void);

		IAttribute* GetAttribute( AttributeNameKey attributeNameKey );
		void DisableDraw(void);
		void EnableDraw(void);

		Scene* scene;

	public:
		
		enum GameObjectFlags{ AABB_INVALID = 1, STATIC = 2, ASLEEP = 4, UPDATEABLE = 8, DELETING = 16, UNIQUE = 32, MOVED = 64, NO_DRAW = 128 };

		GameObject(void);

		~GameObject();

		inline unsigned int GetGameObjectFlags(void) const { return gameObjectFlags; }

		const EDMath::Aabb& GetAabb(void);
		const EDMath::Aabb& GetLocalAabb(void){ return localAabb; }

		static void CollideObjects(GameObject* objectA, GameObject* objectB);

		void TranslateLocal(const EDMath::Float3 &delta);
		void TranslateGlobal(const EDMath::Float3 &delta);
		
		void RotateLocalX(float radians);
		void RotateLocalY(float radians);
		void RotateLocalZ(float radians);

		void RotateGlobalX(float radians);
		void RotateGlobalY(float radians);
		void RotateGlobalZ(float radians);

		void SetLocalTransform(const EDMath::Float4x4 &transform);
		void SetLocalTransform(EDMath::Float4x4 &&transform);
		void SetLocalPosition(const EDMath::Float3 &position);

		void AddBehavior( BehaviorNameKey behaviorNameKey );
		void RemoveBehavior( BehaviorNameKey behaviorNameKey );

		void OnMessage( IMessage* msg );
		void OnBroadcastMessage( IMessage* msg );
		void OnSendMessageUp( IMessage* msg );

		void BuildLocalAabb(void);

		GameObject* GetParentObject(void);
		GameObject* GetRootObject(void);
		GameObject* GetOwner(void)  { return ownerPtr; }

		NodeType GetNodeType(void){ return GAME_OBJECT; }
		
		void SetLocalTransform(TransformKey xformKey, const EDMath::Float4x4 &xform);
		const EDMath::Float4x4 &GetLocalTransform(TransformKey xformKey) const;
		const EDMath::Float4x4 &GetLocalTransform(void)const;

		const EDMath::Float4x4 &GetWorldTransform(TransformKey xformKey);
		const EDMath::Float4x4 &GetWorldTransform(void);

		bool AddChildObject(GameObject *child, TransformKey xformKey);
		void RemoveChildObject(GameObject *child);

		GameObject *SpawnSubObject(const char *xmlFileNamePtr, const EDMath::Float4x4 *xform = 0);


		template<typename type>
		class GOCAttribute
		{
			friend class GameObject;
		private:
			GameObject* gameObject;
			AttributeNameKey attribNameKey;

			GOCAttribute(GameObject* object, AttributeNameKey nameKey) : 
				gameObject(object), attribNameKey(nameKey) {}

		public:

			operator type() const
			{
				return ((Attribute<type>*)(gameObject->GetAttribute(attribNameKey)))->value;
			}

			GOCAttribute & operator=(const type& rhs)
			{
				((Attribute<type>*)(gameObject->GetAttribute(attribNameKey)))->value = rhs;
				gameObject->OnMessage( &MessageT<AttributeNameKey>( MSG_ATTRIBUTE_CHANGED,
					attribNameKey ) );

				return *this;
			}
		};

		template<typename type>
		GOCAttribute<type> GetGOCAttribute(AttributeNameKey attributeNameKey)
		{
			return GOCAttribute<type>( this, attributeNameKey );
		}
	};
}