#include "GameObject.h"
#include "Transform.h"
#include "Behavior.h"
#include "Game.h"
#include "Renderer.h"
#include "Collider.h"
#include "RigidBody.h"
#include "Light.h"
#include "Component.h"
#include "Message.h"
#include "Camera.h"
#include "Animation.h"
#include <assert.h>

namespace EDGameCore
{
	list<GameObject*> GameObject::gameObjectList;
	unordered_map<unsigned int, GameObject*> GameObject::instanceMap;
	unsigned int GameObject::instanceCounter = 0;

	RegistrationId GameObject::typeId = 0;
	string GameObject::typeName;	

	GameObject* GameObject::GetGameObjectInstance(unsigned int instanceId)
	{
		auto iter = instanceMap.find( instanceId );

		if( iter != instanceMap.end() && !iter->second->isDestroying() )
			return iter->second;

		return 0;
	}

	GameObject::GameObject(void)
	{
		layer = 0;
		nameHash = 0;
		memset( components, 0, sizeof(components) );
		transform = Game::create<Transform>();
		transform->gameObject = this;
		instanceId = 0;

		disable();
	}

	GameObject::GameObject(const GameObject& rhs)
	{
		name = rhs.name;
		nameHash = rhs.nameHash;
		layer = rhs.layer;
		tags = rhs.tags;
		memset( components, 0, sizeof(components) );

		transform = rhs.transform->clone();
		transform->gameObject = this;

		if( rhs.renderer != 0 )
			SetRenderer( rhs.renderer->clone() );

		if( rhs.collider != 0 )
			SetCollider( rhs.collider->clone() );

		if( rhs.rigidBody != 0 )
			SetRigidBody( rhs.rigidBody->clone() );

		if( rhs.light != 0 )
			SetLight( rhs.light->clone() );

		if( rhs.camera != 0 )
			SetCamera( rhs.camera->clone() );

		if( rhs.animation != 0 )
			SetAnimation( rhs.animation->clone() );

		auto iter = rhs.behaviorMap.begin();
		auto end = rhs.behaviorMap.end();
		
		for(; iter != end; ++iter)
			AddBehavior( iter->second->clone() );

		instanceId = ++instanceCounter;
		instanceMap.insert( make_pair( instanceId, this ) );

		if( !rhs.DestroysOnLoad() )
			Object::DontDestroyOnLoad(this);	

		disable();
	}

	GameObject::~GameObject()
	{
		if( instanceId != 0 )
		{
			auto iter = instanceMap.find( instanceId );

			if( iter != instanceMap.end() )
				instanceMap.erase( iter );

			gameObjectList.remove(this);
		}
	}

	void GameObject::OnMessage(Message& message)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			behaviorIter->second->OnMessage(message);
	}
	void GameObject::OnMessage(Message& message, const void * data)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
		{
			if (nullptr == data)
			{
				behaviorIter->second->OnMessage(message);
			}
			else
			{
				behaviorIter->second->OnMessage(message, data);
			}
		}
	}
	void GameObject::BroadcastMessage(Message& message)
	{
		OnMessage( message );

		Transform* current = transform->firstChild;

		while( current != 0 )
		{
			current->gameObject->BroadcastMessage(message);
			current = current->nextSibling;
		}
	}

	void GameObject::SendMessageUp(Message& message)
	{
		OnMessage( message );

		transform->parent->gameObject->BroadcastMessage(message);
	}

	GameObject* GameObject::Find(const char* name)
	{
		char string[64];
		strcpy_s( string, name );
		char seps[] = "\\/:";
		char* token = 0;
		char* next_token = 0;

		token = strtok_s( string, seps, &next_token );
		
		if(token == 0)
			return 0;

		RegistrationId nameHash = (unsigned int)std::hash<std::string>()(token);

		auto iter = gameObjectList.begin();
		auto end = gameObjectList.end();

		for(; iter != end; ++iter)
			if( (*iter)->GetNameHash() == nameHash )
				break;

		if( iter == end )
			return 0;

		GameObject* gameObject = (*iter);

		token = strtok_s( 0, seps, &next_token );
		
		while( token != 0 )
		{
			nameHash = (unsigned int)std::hash<std::string>()(token);

			gameObject = gameObject->GetTransform()->Find( nameHash );

			if( gameObject == 0 )
				return 0;

			token = strtok_s( 0, seps, &next_token );
		}

		return gameObject;
	}

	IBehavior* GameObject::AddBehavior(IBehavior* b)
	{
		if( b->GetGameObject() != 0 )
			return 0;

		auto behaviorIter = behaviorMap.find(b->GetTypeId());

		if( behaviorIter != behaviorMap.end() )
			Game::destroy( behaviorIter->second );

		b->gameObject = this;
		behaviorMap.insert( make_pair( b->GetTypeId(), b ) );

		RegistrationId* behaviors = b->GetRequiredBehaviors();
			
		if( behaviors != 0 )
		{
			while( *behaviors != 0 )
			{
				AddBehavior( *behaviors );
				behaviors += 1;
			}
		}

		return b;
	}

	IBehavior* GameObject::AddBehavior(RegistrationId id)
	{
		auto behaviorIter = behaviorMap.find(id);

		if( behaviorIter == behaviorMap.end() )
		{
			IBehavior* behaviorPtr = (IBehavior*)Game::create(id);

			behaviorMap.insert( pair<RegistrationId, IBehavior*>( id, behaviorPtr ) );

			RegistrationId* behaviors = behaviorPtr->GetRequiredBehaviors();
			
			if( behaviors != 0 )
			{
				while( *behaviors != 0 )
				{
					AddBehavior( *behaviors );
					behaviors += 1;
				}
			}

			return behaviorPtr;
		}

		return behaviorIter->second;
	}

	IBehavior* GameObject::AddBehavior(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		if( id != 0 )
			return AddBehavior(id);

		return 0;
	}

	bool GameObject::HasBehavior(RegistrationId id)
	{
		auto behaviorIter = behaviorMap.find(id);

		return behaviorIter != behaviorMap.end();
	}

	bool GameObject::HasBehavior(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		if( id != 0 )
			return HasBehavior(id);

		return 0;
	}

	const GameObject* GameObject::GetParent(void)const
	{
		const Transform* parentXform = transform->GetParent();

		return parentXform == 0 ? 0 : parentXform->GetGameObject();
	}

	bool GameObject::isEnabledInHierarchy(void)const
	{
		const GameObject* currentObj = this;

		while( currentObj != 0 )
		{
			if( currentObj->isEnabled() == false )
				return false;

			currentObj = currentObj->GetParent();
		}

		return true;
	}

	void GameObject::Update(void)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
		{
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->Update();
		}

		if( camera != 0 && camera->isEnabled() )
			camera->Update();

		if( animation != 0 && animation->isEnabled() && !animation->AnimatesPhysics() )
			animation->Update();
	}

	void GameObject::FixedUpdate(void)
	{
		if( collider != 0 && collider->isEnabled() )
			collider->lastPosition = collider->GetAabb().center;

		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
		{
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->FixedUpdate();
		}

		if( animation != 0 && animation->isEnabled() && animation->AnimatesPhysics() )
			animation->Update();

		if( rigidBody != 0 && rigidBody->isEnabled() )
			rigidBody->Update();
	}

	void GameObject::LateUpdate(void)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
		{
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->LateUpdate();
		}
	}

	void GameObject::SetLayer(const char* identifier)
	{
		SetLayer( Game::GetStringRegistrationId(identifier) );
	}

	void GameObject::SetName(const char* n)
	{
		name = n;
		nameHash = Game::RegisterString(n);
	}

	void GameObject::AddTag(const char* identifier)
	{
		AddTag( Game::RegisterString(identifier) );
	}

	void GameObject::AddTag(RegistrationId id)
	{
		if( id != 0 )
			tags.insert(id);
	}

	bool GameObject::HasTag(const char* identifier)
	{
		return HasTag( Game::GetStringRegistrationId(identifier) );
	}

	bool GameObject::HasTag(RegistrationId id)
	{
		return (tags.find(id) != tags.end());
	}

	void GameObject::RemoveTag(const char* identifier)
	{
		RemoveTag( Game::GetStringRegistrationId(identifier) );
	}

	void GameObject::RemoveTag(RegistrationId id)
	{
		tags.erase(id);
	}

	Animation* GameObject::SetAnimation(Animation* a)
	{
		if( a != 0 && a->GetGameObject() != 0 )
			return 0;

		if( animation != 0 )
			Game::destroy( animation );

		animation = a;

		if( animation != 0 )
		{
			animation->gameObject = this;
		}

		return animation;
	}

	Animation* GameObject::SetAnimation(RegistrationId id)
	{	
		Animation* newAnim = 0;

		if( id != 0 )
			newAnim = (Animation*)Game::create(id);

		return SetAnimation( newAnim );
	}

	Animation* GameObject::SetAnimation(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		return SetAnimation(id);
	}

	IRenderer* GameObject::SetRenderer(IRenderer* r)
	{
		if( r != 0 && r->GetGameObject() != 0 )
			return 0;

		if( renderer != 0 )
			Game::destroy( renderer );

		renderer = r;

		if( renderer != 0 )
		{
			renderer->gameObject = this;
		}

		return renderer;
	}

	IRenderer* GameObject::SetRenderer(RegistrationId id)
	{	
		IRenderer* newRenderer = 0;

		if( id != 0 )
			newRenderer = (IRenderer*)Game::create(id);

		return SetRenderer( newRenderer );
	}

	IRenderer* GameObject::SetRenderer(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		return SetRenderer(id);
	}

	Camera* GameObject::SetCamera(Camera* c)
	{
		if( c != 0 && c->GetGameObject() != 0 )
			return 0;

		if( camera != 0 )
			Game::destroy( camera );

		camera = c;

		if( camera != 0 )
		{
			camera->gameObject = this;
		}

		return camera;
	}

	Camera* GameObject::SetCamera(RegistrationId id)
	{	
		Camera* newCamera = 0;

		if( id != 0 )
			newCamera = (Camera*)Game::create(id);

		return SetCamera( newCamera );
	}

	Camera* GameObject::SetCamera(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		return SetCamera(id);
	}

	ILight* GameObject::SetLight(ILight* l)
	{
		if( l != 0 && l->GetGameObject() != 0 )
			return 0;

		if( light != 0 )
			Game::destroy( light );

		light = l;

		if( light != 0 )
		{
			light->gameObject = this;
		}

		return light;
	}

	ILight* GameObject::SetLight(RegistrationId id)
	{
		ILight* newLight = 0;

		if( id != 0 )
			newLight = (ILight*)Game::create(id);
		
		return SetLight( newLight );
	}

	ILight* GameObject::SetLight(const char* identifier)
	{
		return SetLight( Game::GetStringRegistrationId(identifier) );
	}

	ICollider* GameObject::SetCollider(ICollider* l)
	{
		if( l != 0 && l->GetGameObject() != 0 )
			return 0;

		if( collider != 0 )
			Game::destroy( collider );

		collider = l;

		if( collider != 0 )
		{
			collider->gameObject = this;
		}

		return collider;
	}

	ICollider* GameObject::SetCollider(RegistrationId id)
	{	
		ICollider* newCollider = 0;
		
		if( id != 0 )
			newCollider = (ICollider*)Game::create(id);

		return SetCollider(newCollider);
	}

	ICollider* GameObject::SetCollider(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		return SetCollider(id);
	}

	RigidBody* GameObject::SetRigidBody(RigidBody* l)
	{
		if( l != 0 && l->GetGameObject() != 0 )
			return 0;

		if( rigidBody != 0 )
			Game::destroy( rigidBody );

		rigidBody = l;

		if( rigidBody != 0 )
		{
			rigidBody->gameObject = this;
		}

		return rigidBody;
	}

	RigidBody* GameObject::SetRigidBody(RegistrationId id)
	{
		RigidBody* newRigidBody = 0;
		
		if( id != 0 )
			newRigidBody = (RigidBody*)Game::create(id);

		return SetRigidBody( newRigidBody );
	}

	RigidBody* GameObject::SetRigidBody(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		return SetRigidBody(id);
	}

	void GameObject::OnDestroy(void)
	{
		auto iter = behaviorMap.begin();
		auto end = behaviorMap.end();

		for(; iter != end; ++iter)
			Game::destroy( iter->second );

		for(int i = 0; i < COMPONENT_COUNT; ++i)
		{
			if( components[i] != 0 )
				Game::destroy( components[i] );
		}
	}

	void GameObject::RemoveBehavior(RegistrationId id)
	{
		behaviorMap.erase( GetTypeId() );
	}

	IBehavior* GameObject::GetBehavior(const char* identifier)
	{
		RegistrationId id = Game::GetStringRegistrationId(identifier);
		
		return GetBehavior(id);
	}

	IBehavior* GameObject::GetBehavior(RegistrationId id)
	{
		auto iter = behaviorMap.find( id );

		if( iter != behaviorMap.end() )
			return iter->second;

		return 0;
	}

	void GameObject::OnCollisionEnter(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->OnCollisionEnter( thisCollider, otherCollider, contact );
	}

	void GameObject::OnCollisionStay(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->OnCollisionStay( thisCollider, otherCollider, contact );
	}

	void GameObject::OnCollisionExit(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->OnCollisionExit( thisCollider, otherCollider, contact );
	}

	void GameObject::OnTriggerEnter(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->OnTriggerEnter( thisCollider, otherCollider, contact );
	}

	void GameObject::OnTriggerStay(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->OnTriggerStay( thisCollider, otherCollider, contact );
	}

	void GameObject::OnTriggerExit(ICollider* thisCollider, ICollider* otherCollider, const EDCollision::Contact& contact)
	{
		auto behaviorIter = behaviorMap.begin();
		auto behaviorEnd = behaviorMap.end();

		for(; behaviorIter != behaviorEnd; ++behaviorIter)
			if( behaviorIter->second->isEnabled() )
				behaviorIter->second->OnTriggerExit( thisCollider, otherCollider, contact );
	}

	void GameObject::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* animEle = xmlElement->FirstChildElement("Animation");
		if( animEle != 0 )
		{
			if( animation == 0 )
				SetAnimation( EDGameCore::Game::GetTypeId<Animation>() );

			if( animation != 0 )
				animation->LoadState( animEle );
		}

		TiXmlElement* rendererEle = xmlElement->FirstChildElement("Renderer");

		if( rendererEle != 0 )
		{
			TiXmlElement* typeEle = rendererEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen(typeEle->GetText()) != 0 )
			{
				if( renderer == 0 ||
					strcmp( renderer->GetTypeName(), typeEle->GetText() ) != 0)
				{
					SetRenderer(typeEle->GetText());
				}

				if(renderer != 0)
					renderer->LoadState( rendererEle );
			}
		}

		TiXmlElement* colliderEle = xmlElement->FirstChildElement("Collider");

		if( colliderEle != 0 )
		{
			TiXmlElement* typeEle = colliderEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen(typeEle->GetText()) != 0 )
			{
				if( collider == 0 ||
					strcmp( collider->GetTypeName(), typeEle->GetText() ) != 0 )
				{
					SetCollider(typeEle->GetText());
				}

				if(collider != 0)
				{
					collider->LoadState( colliderEle );

					TiXmlElement* triggerEle = colliderEle->FirstChildElement("Trigger");

					if( triggerEle != 0 )
					{
						bool triggerState = true;

						if( triggerEle->GetText() != 0 &&
							strlen(triggerEle->GetText()) != 0 &&
							_stricmp( triggerEle->GetText(), "true" ) != 0 &&
							_stricmp( triggerEle->GetText(), "1" ) != 0 )
							triggerState = false;

						collider->SetTrigger(triggerState);
					}

					TiXmlElement* physMatEle = colliderEle->FirstChildElement("PhysicsMaterial");
					if( physMatEle != 0 &&
						physMatEle->GetText() != 0 &&
						strlen(physMatEle->GetText()) != 0 )
						collider->SetMaterialId( Game::GetStringRegistrationId(physMatEle->GetText()) );
				}
			}
		}

		TiXmlElement* lightEle = xmlElement->FirstChildElement("Light");

		if( lightEle != 0 )
		{
			TiXmlElement* typeEle = lightEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen(typeEle->GetText()) != 0 )
			{
				if( light == 0 ||
					strcmp( light->GetTypeName(), typeEle->GetText() ) != 0 )
				{
					SetLight(typeEle->GetText());
				}

				if(light != 0)
					light->LoadState( lightEle );
			}
		}

		TiXmlElement* rigidBodyEle = xmlElement->FirstChildElement("RigidBody");
		
		if( rigidBodyEle != 0 )
		{
			if( rigidBody == 0 )
				SetRigidBody( EDGameCore::Game::GetTypeId<RigidBody>() );

			if( rigidBody != 0 )
				rigidBody->LoadState( rigidBodyEle );
		}

		TiXmlElement* cameraEle = xmlElement->FirstChildElement("Camera");
		if( cameraEle != 0 )
		{
			if( camera == 0 )
				SetCamera( EDGameCore::Game::GetTypeId<Camera>() );

			if( camera != 0 )
				camera->LoadState( cameraEle );
		}

		TiXmlElement* behaviorsEle = xmlElement->FirstChildElement("Behaviors");
		
		if( behaviorsEle != 0 )
		{
			TiXmlElement* currentEle = behaviorsEle->FirstChildElement();

			for(; currentEle != 0; currentEle = currentEle->NextSiblingElement())
			{
				IBehavior* behavior = 0;
				if( !HasBehavior(currentEle->Value()) )
				{
					behavior = AddBehavior( currentEle->Value() );
				}
				else
				{
					behavior = GetBehavior(currentEle->Value());					
				}

				if( behavior != 0 )
					behavior->LoadState(currentEle);
			}
		}

		TiXmlElement* tagEle = xmlElement->FirstChildElement("Tag");

		for(; tagEle != 0; tagEle = tagEle->NextSiblingElement("Tag") )
		{
			if( tagEle->GetText() != 0 &&
				strlen( tagEle->GetText() ) != 0 )
			{
				AddTag( tagEle->GetText() );
			}
		}

		TiXmlElement* nameEle = xmlElement->FirstChildElement("Name");

		if( nameEle != 0 &&
			nameEle->GetText() != 0 &&
			strlen(nameEle->GetText()) != 0 )
		{
			SetName(nameEle->GetText());
		}

		TiXmlElement* layerEle = xmlElement->FirstChildElement("Layer");
		if( layerEle != 0 &&
			layerEle->GetText() != 0 &&
			strlen(layerEle->GetText()) != 0 )
		{
			SetLayer(layerEle->GetText());
		}
		else
			SetLayer("Default");

		TiXmlElement* childrenEle = xmlElement->FirstChildElement("Children");
		
		if( childrenEle != 0 )
		{
			TiXmlElement* currentEle = childrenEle->FirstChildElement("GameObject");
			for(; currentEle != 0; currentEle = currentEle->NextSiblingElement("GameObject"))
			{
				TiXmlElement* parentEle = currentEle->FirstChildElement("Parent");
				
				EDGameCore::Transform* parentTransform = GetTransform();

				if( parentEle != 0 &&
					parentEle->GetText() != 0 &&
					strlen( parentEle->GetText() ) != 0 )
				{
					EDGameCore::GameObject* parentObj = GetTransform()->FindInHierarchy(parentEle->GetText());
					
					if( parentObj != 0 )
						parentTransform = parentObj->GetTransform();
					else
						parentTransform = 0;
				}

				if( parentTransform != 0 )
				{
					GameObject* child = Load( currentEle );
				
					if( child != 0 )
						child->GetTransform()->SetParent( parentTransform );
				}
			}
		}

		TiXmlElement* transformEle = xmlElement->FirstChildElement("Transform");

		if( transformEle != 0 )
			GetTransform()->LoadState(transformEle);

		TiXmlElement* destroyEle = xmlElement->FirstChildElement("DestroyOnLoad");

		if( destroyEle != 0 &&
			destroyEle->GetText() != 0 &&
			strlen( destroyEle->GetText() ) != 0 )
		{
			if( !_stricmp( destroyEle->GetText(), "false" ) ||
				!strcmp( destroyEle->GetText(), "0" ) )
				Object::DontDestroyOnLoad(this);
		}
	}

	TiXmlElement* GetRootGameObject(TiXmlDocument& doc, const char* fileName)
	{
		TiXmlElement* elementPtr = Game::LoadDoc(doc, fileName);

		if( elementPtr != 0 )
		{
			std::string rootName = elementPtr->Value();

			if( strcmp(rootName.c_str(), "GameObject") != 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing a GameObject element\n", 
					fileName);
				return 0;
			}
		}

		return elementPtr;
	}

	GameObject* GameObject::Load(const char* fileName)
	{
		TiXmlDocument doc;
		TiXmlElement* elementPtr = GetRootGameObject(doc, fileName);

		if( elementPtr == 0 )
			return 0;

		return Load(elementPtr);
	}

	GameObject* GameObject::Load(TiXmlElement* xmlElement)
	{
		TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

		GameObject* gameObj = 0;

		if( pathEle != 0 )
		{
			if( pathEle->GetText() != 0 &&
				strlen( pathEle->GetText() ) != 0 )
			{
				gameObj = Load( pathEle->GetText() );
			}
		}

		if( gameObj == 0 )
			gameObj = Game::create<GameObject>();

		gameObj->LoadState(xmlElement);

		return gameObj;

		/*
		TiXmlElement* animEle = xmlElement->FirstChildElement("Animation");
		if( animEle != 0 )
		{
			Animation* animation = gameObj->SetAnimation( EDGameCore::Game::GetTypeId<Animation>() );
			if( animation != 0 )
				animation->LoadState( animEle );
		}

		TiXmlElement* rendererEle = xmlElement->FirstChildElement("Renderer");

		if( rendererEle != 0 )
		{
			TiXmlElement* typeEle = rendererEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen(typeEle->GetText()) != 0 )
			{
				IRenderer* renderer = gameObj->SetRenderer(typeEle->GetText());
				if(renderer != 0)
					renderer->LoadState( rendererEle );
			}
		}

		TiXmlElement* colliderEle = xmlElement->FirstChildElement("Collider");

		if( colliderEle != 0 )
		{
			TiXmlElement* typeEle = colliderEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen(typeEle->GetText()) != 0 )
			{
				ICollider* collider = gameObj->SetCollider(typeEle->GetText());
				if(collider != 0)
				{
					collider->LoadState( colliderEle );

					TiXmlElement* triggerEle = colliderEle->FirstChildElement("Trigger");

					if( triggerEle != 0 )
					{
						bool triggerState = true;

						if( triggerEle->GetText() != 0 &&
							strlen(triggerEle->GetText()) != 0 &&
							_stricmp( triggerEle->GetText(), "true" ) != 0 &&
							_stricmp( triggerEle->GetText(), "1" ) != 0 )
							triggerState = false;

						collider->SetTrigger(triggerState);
					}

					TiXmlElement* physMatEle = colliderEle->FirstChildElement("PhysicsMaterial");
					if( physMatEle != 0 &&
						physMatEle->GetText() != 0 &&
						strlen(physMatEle->GetText()) != 0 )
						collider->SetMaterialId( Game::GetStringRegistrationId(physMatEle->GetText()) );
				}
			}
		}

		TiXmlElement* lightEle = xmlElement->FirstChildElement("Light");

		if( lightEle != 0 )
		{
			TiXmlElement* typeEle = lightEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen(typeEle->GetText()) != 0 )
			{
				ILight* light = gameObj->SetLight(typeEle->GetText());
				if(light != 0)
					light->LoadState( lightEle );
			}
		}

		TiXmlElement* rigidBodyEle = xmlElement->FirstChildElement("RigidBody");
		
		if( rigidBodyEle != 0 )
		{
			RigidBody* rigidBody = gameObj->SetRigidBody( EDGameCore::Game::GetTypeId<RigidBody>() );
			if( rigidBody != 0 )
				rigidBody->LoadState( rigidBodyEle );
		}

		TiXmlElement* cameraEle = xmlElement->FirstChildElement("Camera");
		if( cameraEle != 0 )
		{
			Camera* camera = gameObj->SetCamera( EDGameCore::Game::GetTypeId<Camera>() );
			if( camera != 0 )
				camera->LoadState( cameraEle );
		}

		TiXmlElement* behaviorsEle = xmlElement->FirstChildElement("Behaviors");
		
		if( behaviorsEle != 0 )
		{
			TiXmlElement* currentEle = behaviorsEle->FirstChildElement();

			for(; currentEle != 0; currentEle = currentEle->NextSiblingElement())
			{
				if( !gameObj->HasBehavior(currentEle->Value()) )
				{
					IBehavior* behavior = gameObj->AddBehavior( currentEle->Value() );
					if( behavior != 0 )
						behavior->LoadState( currentEle );
				}
			}
		}

		TiXmlElement* tagEle = xmlElement->FirstChildElement("Tag");

		for(; tagEle != 0; tagEle = tagEle->NextSiblingElement("Tag") )
		{
			if( tagEle->GetText() != 0 &&
				strlen( tagEle->GetText() ) != 0 )
			{
				gameObj->AddTag( tagEle->GetText() );
			}
		}

		TiXmlElement* nameEle = xmlElement->FirstChildElement("Name");

		if( nameEle != 0 &&
			nameEle->GetText() != 0 &&
			strlen(nameEle->GetText()) != 0 )
		{
			gameObj->SetName(nameEle->GetText());
		}

		TiXmlElement* layerEle = xmlElement->FirstChildElement("Layer");
		if( layerEle != 0 &&
			layerEle->GetText() != 0 &&
			strlen(layerEle->GetText()) != 0 )
		{
			gameObj->SetLayer(layerEle->GetText());
		}
		else
			gameObj->SetLayer("Default");

		TiXmlElement* childrenEle = xmlElement->FirstChildElement("Children");
		
		if( childrenEle != 0 )
		{
			TiXmlElement* currentEle = childrenEle->FirstChildElement("GameObject");
			for(; currentEle != 0; currentEle = currentEle->NextSiblingElement("GameObject"))
			{
				TiXmlElement* parentEle = currentEle->FirstChildElement("Parent");
				
				EDGameCore::Transform* parentTransform = gameObj->GetTransform();

				if( parentEle != 0 &&
					parentEle->GetText() != 0 &&
					strlen( parentEle->GetText() ) != 0 )
				{
					EDGameCore::GameObject* parentObj = gameObj->GetTransform()->FindInHierarchy(parentEle->GetText());
					
					if( parentObj != 0 )
						parentTransform = parentObj->GetTransform();
					else
						parentTransform = 0;
				}

				if( parentTransform != 0 )
				{
					GameObject* child = Load( currentEle );
				
					if( child != 0 )
						child->GetTransform()->SetParent( parentTransform );
				}
			}
		}

		TiXmlElement* transformEle = xmlElement->FirstChildElement("Transform");

		if( transformEle != 0 )
			gameObj->GetTransform()->LoadState(transformEle);

		TiXmlElement* destroyEle = xmlElement->FirstChildElement("DestroyOnLoad");

		if( destroyEle != 0 &&
			destroyEle->GetText() != 0 &&
			strlen( destroyEle->GetText() ) != 0 )
		{
			if( !_stricmp( destroyEle->GetText(), "false" ) ||
				!strcmp( destroyEle->GetText(), "0" ) )
				Object::DontDestroyOnLoad(gameObj);
		}

		return gameObj;*/
	}

	void GameObject::OnEnable(void)
	{
		auto iter = behaviorMap.begin();
		auto end = behaviorMap.end();

		for(; iter != end; ++iter)
		{
			if( iter->second->isEnabled() )
				iter->second->OnEnable();
		}

		for(int i = 0; i < COMPONENT_COUNT; ++i)
		{
			if( components[i] != 0 )
			{
				if( components[i]->isEnabled() )
					components[i]->OnEnable();
			}
		}

		Transform* transform = GetTransform()->firstChild;
		for(; transform != 0; transform = transform->nextSibling )
		{
			if( transform->GetGameObject()->isEnabled() )
				transform->GetGameObject()->OnEnable();
		}
	}

	void GameObject::OnDisable(void)
	{
	}

	void GameObject::Awake(void)
	{
		auto iter = behaviorMap.begin();
		auto end = behaviorMap.end();

		for(; iter != end; ++iter)
			iter->second->Awake();
	}

	unsigned int GameObject::GetLayerMask(void)const
	{
		return Game::GetLayerMask( GetLayer() );
	}
}