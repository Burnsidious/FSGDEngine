#include "precompiled.h"
#include "GameObject.h"

#include "../EDCollision/Collision.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "ComponentSystem.h"
#include "MessageT.h"
#include "DynAabbTreeNode.h"
#include "DynAabbTree.h"

#include "Scene.h"
#include "../tinyxml/tinyxml.h"
#include "../EDUtilities/XMLLoadingMethods.h"
#include "../EDUtilities/Hasher.h"

using namespace std;
using namespace EDUtilities;
using namespace EDGOCA;
using namespace EDMath;
using namespace EDCollision;

namespace GameObjectMessages
{
	DeclareMessage( MSG_COLLIDE );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_ATTRIBUTE_CHANGED );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

using namespace GameObjectMessages;

namespace EDGOCA
{
	GameObjectTemplate* GameObjectTemplate::LoadXML(const char* xmlFilePtr, const char* indentifier)
	{
		string temp = ContentManager::theContentPath;
		temp += xmlFilePtr;
		const char* xmlFileNamePtr = temp.c_str();

		InternalOutput::GetReference() << "Loading " << xmlFileNamePtr << "...\n";

		TiXmlDocument doc(xmlFileNamePtr);

		if(!doc.LoadFile())
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
			return 0;
		}

		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;
		TiXmlHandle hRoot(0);
		
		elementPtr = hDoc.FirstChildElement().Element();
		if( !elementPtr )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, is the file empty?\n", xmlFileNamePtr);
			return 0;
		}

		std::string rootName = elementPtr->Value();

		if( strcmp(rootName.c_str(), "GameObject") != 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GameObject element\n", 
				xmlFileNamePtr);
			return 0;
		}

		GameObjectTemplate* templatePtr = new GameObjectTemplate();

		templatePtr->aabb.min = Float3( -0.5f, -0.5f, -0.5f );
		templatePtr->aabb.max = Float3( 0.5f, 0.5f, 0.5f );

		// TODO: Why was it hashValue +1?
		templatePtr->id = static_cast<unsigned int>(Hasher::HashString( temp )/* + 1*/);
		templatePtr->flags = 0;

		TiXmlElement* flagsEle = elementPtr->FirstChildElement("Flags");

		if( flagsEle != 0 )
		{
			TiXmlElement* staticFlagEle = flagsEle->FirstChildElement("Static");
			if( staticFlagEle != 0 )
				if( staticFlagEle->GetText() != 0 )
					if( strcmp( staticFlagEle->GetText(), "1" ) == 0 )
						templatePtr->flags |= GameObject::STATIC;

			TiXmlElement* UpdateableEle = flagsEle->FirstChildElement("Updateable");
			if( UpdateableEle != 0 )
				if( UpdateableEle->GetText() != 0 )
					if( strcmp( UpdateableEle->GetText(), "1" ) == 0 )
					{
						templatePtr->flags &= ~GameObject::STATIC;
						templatePtr->flags |= GameObject::UPDATEABLE;
					}

			TiXmlElement* uniqueFlag = flagsEle->FirstChildElement("Unique");
			if( uniqueFlag != 0 )
				if( uniqueFlag->GetText() != 0 )
					if( strcmp( uniqueFlag->GetText(), "1" ) == 0 )
						templatePtr->flags |= GameObject::UNIQUE;
		}

		TiXmlElement* bhvrListEle = elementPtr->FirstChildElement("Behavior_List");
		
		if( bhvrListEle == 0 )
			return templatePtr;

		TiXmlElement* bhvrEle = bhvrListEle->FirstChildElement("Behavior");

		for(; bhvrEle != 0; bhvrEle = bhvrEle->NextSiblingElement("Behavior") )
		{
			if( bhvrEle->GetText() == 0 )
				continue;

			BehaviorNameKey nameKey = ComponentSystem::GetBehaviorNameKey( bhvrEle->GetText() );

			if( nameKey == 0 )
				continue;

			templatePtr->behaviors.insert( nameKey );
		}

		TiXmlElement* attribListEle = elementPtr->FirstChildElement("Attribute_List");
		
		if( attribListEle == 0 )
			return templatePtr;

		TiXmlElement* attribEle = attribListEle->FirstChildElement("Attribute");

		for(; attribEle != 0; attribEle = attribEle->NextSiblingElement("Attribute") )
		{
			TiXmlElement* nameEle = attribEle->FirstChildElement("Name");
			
			if( nameEle == 0 )
				continue;
			if( nameEle->GetText() == 0 )
				continue;

			AttributeNameKey nameKey = ComponentSystem::GetAttributeNameKey( nameEle->GetText() );

			if( nameKey == 0 )
				continue;

			IAttribute* attributePtr = ComponentSystem::CreateAttribute(nameKey);

			templatePtr->attributes.insert( pair< AttributeNameKey, IAttribute* >( nameKey, attributePtr ) );

			TiXmlElement* valEle = attribEle->FirstChildElement("Value");

			if( valEle == 0 )
				continue;

			attributePtr->ReadXML(valEle);
		}

		TiXmlElement* xformSetEle = elementPtr->FirstChildElement("TransformSet");
		
		if( xformSetEle != 0 )
		{
			TiXmlElement* xformEle = xformSetEle->FirstChildElement("Transform");
			
			for(; xformEle != 0; xformEle = xformEle->NextSiblingElement("Transform"))
			{
				TiXmlElement* xformKeyEle = xformEle->FirstChildElement("TransformName");
				TiXmlElement* matrixEle = xformEle->FirstChildElement("LocalMatrix");
				TiXmlElement* xformFlagEle = xformEle->FirstChildElement("TransformFlag");
				
				if( xformKeyEle == 0 || matrixEle == 0 )
					continue;

				//pair< TransformKey, Float4x4 > node;
				TransformData node;
				node.transformKey = static_cast<TransformKey>(Hasher::HashString( xformKeyEle->GetText() ));

				node.localMatrix.makeIdentity();
				ReadXmlMatrix( matrixEle, &node.localMatrix );

				if(xformFlagEle)
				{
					string xformFlagString = xformFlagEle->GetText();
					if("INHERIT_XFORM_POS" == xformFlagString)
						node.transformFlag = TransformNode::INHERIT_XFORM_POS;
					else if("INHERIT_XFORM_ROT" == xformFlagString)
						node.transformFlag = TransformNode::INHERIT_XFORM_ROT;
					else if("INHERIT_XFORM" == xformFlagString)
						node.transformFlag = TransformNode::INHERIT_XFORM;
				}
				else
					node.transformFlag = TransformNode::INHERIT_XFORM;

				templatePtr->transforms.push_back( node );
			}
		}

		TiXmlElement* childObjectList = elementPtr->FirstChildElement("Child_List");

		if( childObjectList != 0 )
		{
			TiXmlElement* childEle = childObjectList->FirstChildElement("Child");
			for(; childEle != 0; childEle = childEle->NextSiblingElement("Child"))
			{
				TiXmlElement* pathEle = childEle->FirstChildElement("Path");
				if( pathEle == 0 )
					continue;
				if( pathEle->GetText() == 0 )
					continue;

				TiXmlElement* xformKeyEle = childEle->FirstChildElement("TransformName");
				if( xformKeyEle == 0 )
					continue;

				TransformKey xformKey = -1;
				xformKey = static_cast<TransformKey>(Hasher::HashString( xformKeyEle->GetText() ));

				templatePtr->childObjects.push_back( pair< string, TransformKey >( pathEle->GetText(), xformKey ) );
			}
		}

		return templatePtr;
	}

	GameObject::GameObject(void)
	{
		dynAabbTreeLeaf = 0;
		visLeaf = 0;
		gameObjectFlags = 0;
		scene = 0;
		aabb.min = Float3( FLT_MAX, FLT_MAX, FLT_MAX );
		aabb.max = Float3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		localAabb = aabb;
		ownerPtr = nullptr;
	}

	GameObject::~GameObject()
	{
		while( !behaviors.empty() )
			RemoveBehavior( behaviors.begin()->first );

		GameObject* parentObj = GetParentObject();
		if( parentObj != 0 )
			parentObj->RemoveChildObject(this);

		while( !transformSet.empty() )
		{
			delete transformSet.begin()->second;
			transformSet.erase( transformSet.begin() );
		}
	}

	void GameObject::DirtyNode(void)
	{
		TransformNode::DirtyNode();

		gameObjectFlags |= AABB_INVALID;
		gameObjectFlags |= MOVED;
	}

	const Aabb& GameObject::GetAabb(void)
	{
		if( (gameObjectFlags & AABB_INVALID) == AABB_INVALID )
		{
			const Aabb& _localAabb = localAabb;
			const Float4x4& worldTransform = GetWorldTransform();

			for(unsigned int i = 0; i < 3; ++i)
			{
				aabb.min.v[i] = aabb.max.v[i] = worldTransform.WAxis.v[i];

				for(unsigned int j = 0; j < 3; ++j)
				{
					float e = worldTransform.m[j][i] * _localAabb.min.v[j];
					float f = worldTransform.m[j][i] * _localAabb.max.v[j];

					if( e < f )
					{
						aabb.min.v[i] += e;
						aabb.max.v[i] += f;
					}
					else
					{
						aabb.min.v[i] += f;
						aabb.max.v[i] += e;
					}
				}
			}

			gameObjectFlags &= ~AABB_INVALID;
		}

		return aabb;
	}

	void GameObject::CollideObjects(GameObject* objectA, GameObject* objectB)
	{
		if( objectA->GetGameObjectFlags() & DELETING )
			return;
		if( objectB->GetGameObjectFlags() & DELETING )
			return;

		if( objectA->GetRoot() == objectB->GetRoot() )
			return;

		if( AABBAABBCollide( objectA->GetAabb(), objectB->GetAabb() ) )
		{
			objectA->OnMessage( &MessageT<GameObject*>( MSG_COLLIDE, objectB ) );
//			DebugRenderer::GetInstance()->DrawAabb( objectA->GetAabb() );
		}
	}

	void GameObject::SetLocalTransform(TransformKey xformKey, const Float4x4& xform)
	{
		map<TransformKey, TransformNode*>::iterator xformIter = transformSet.find(xformKey);

		if( xformIter != transformSet.end() )
		{
			xformIter->second->SetLocalTransform(xform);
			return;
		}
	}

	const Float4x4& GameObject::GetLocalTransform(void) const
	{
		return TransformNode::GetLocalTransform();
	}

	const Float4x4& GameObject::GetLocalTransform(TransformKey xformKey) const
	{
		map<TransformKey, TransformNode*>::const_iterator xformIter = transformSet.find(xformKey);

		if( xformIter != transformSet.end() )
			return xformIter->second->GetLocalTransform();

		return GetLocalTransform();
	}

	const Float4x4& GameObject::GetWorldTransform(void)
	{
		return TransformNode::GetWorldTransform();
	}

	const Float4x4& GameObject::GetWorldTransform(TransformKey xformKey)
	{
		map<TransformKey, TransformNode*>::iterator xformIter = transformSet.find(xformKey);

		if( xformIter != transformSet.end() )
			return xformIter->second->GetWorldTransform();
		
		return GetWorldTransform();
	}

	void GameObject::DisableDraw(void)
	{ 
		if( gameObjectFlags & NO_DRAW )
			return;

		gameObjectFlags |= NO_DRAW;

		map< GameObject*, TransformKey >::iterator childIter = childObjects.begin();
		for(; childIter != childObjects.end(); ++childIter)
			childIter->first->DisableDraw();
	}

	void GameObject::EnableDraw(void)
	{ 
		if( (gameObjectFlags & NO_DRAW) == 0 )
			return;

		gameObjectFlags &= ~NO_DRAW;

		map< GameObject*, TransformKey >::iterator childIter = childObjects.begin();
		for(; childIter != childObjects.end(); ++childIter)
			childIter->first->EnableDraw();
	}

	bool GameObject::AddChildObject(GameObject* child, TransformKey xformKey)
	{
		if( (gameObjectFlags & STATIC) != (child->gameObjectFlags & STATIC) )
			return false;

		if( child->GetParent() != 0 )
			return false;
		if( GetRoot() == child )
			return false;

		map<TransformKey, TransformNode*>::iterator xformIter = transformSet.find(xformKey);

		TransformNode* xformNode = 0;
		if( xformIter == transformSet.end() )
		{
			xformNode = this;
			child->DisableDraw();
		}
		else
			xformNode = xformIter->second;

		xformNode->AddChild(child);
		childObjects.insert( pair<GameObject*, TransformKey>( child, xformKey ) );

		return true;
	}

	void GameObject::RemoveChildObject(GameObject* child)
	{
		map<GameObject*, TransformKey>::iterator childIter = childObjects.find(child);
		
		if( childIter == childObjects.end() )
			return;

		childIter->first->GetParent()->RemoveChild(childIter->first);
		childObjects.erase( child );
		child->EnableDraw();
	}

	GameObject *GameObject::SpawnSubObject(const char* xmlFileNamePtr, const Float4x4* xform)
	{
		assert(scene);
		GameObject *goPtr = scene->SpawnObject(xmlFileNamePtr, xform);
		goPtr->ownerPtr = this;
		subObjects.push_back(goPtr);
		return goPtr;
	}

	void GameObject::TranslateLocal(const Float3& delta)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::TranslateLocal(delta);
		gameObjectFlags |= MOVED;
	}

	void GameObject::TranslateGlobal(const Float3& delta)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::TranslateGlobal(delta);
		gameObjectFlags |= MOVED;
	}
		
	void GameObject::RotateLocalX(float radians)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::RotateLocalX(radians);
		gameObjectFlags |= MOVED;
	}

	void GameObject::RotateLocalY(float radians)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::RotateLocalY(radians);
		gameObjectFlags |= MOVED;
	}

	void GameObject::RotateLocalZ(float radians)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::RotateLocalZ(radians);
		gameObjectFlags |= MOVED;
	}

	void GameObject::RotateGlobalX(float radians)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::RotateGlobalX(radians);
		gameObjectFlags |= MOVED;
	}

	void GameObject::RotateGlobalY(float radians)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::RotateGlobalY(radians);
		gameObjectFlags |= MOVED;
	}

	void GameObject::RotateGlobalZ(float radians)
	{
		if( (gameObjectFlags & STATIC) || (gameObjectFlags & UPDATEABLE) == 0 )
			return;

		TransformNode::RotateGlobalZ(radians);
		gameObjectFlags |= MOVED;
	}

	void GameObject::SetLocalTransform(const Float4x4& transform)
	{
		if( (gameObjectFlags & STATIC) /*|| (gameObjectFlags & UPDATEABLE) == 0*/ )
			return;

		TransformNode::SetLocalTransform(transform);

		if( (gameObjectFlags & UPDATEABLE) == UPDATEABLE )
			gameObjectFlags |= MOVED;
	}

	void GameObject::SetLocalTransform(EDMath::Float4x4 &&transform)
	{
		if( (gameObjectFlags & STATIC) /*|| (gameObjectFlags & UPDATEABLE) == 0*/ )
			return;

		TransformNode::SetLocalTransform(std::move(transform));

		if( (gameObjectFlags & UPDATEABLE) == UPDATEABLE )
			gameObjectFlags |= MOVED;
	}

	void GameObject::SetLocalPosition(const Float3& position)
	{
		if( (gameObjectFlags & STATIC) /*|| (gameObjectFlags & UPDATEABLE) == 0*/ )
			return;

		TransformNode::SetLocalPosition(position);

		if( (gameObjectFlags & UPDATEABLE) == UPDATEABLE )
			gameObjectFlags |= MOVED;
	}

	void GameObject::Update(void)
	{
		OnMessage( &IMessage(MSG_UPDATE) );
	}

	GameObject* GameObject::GetParentObject(void)
	{
		GameObject* parentObject = 0;
		TransformNode* parent = GetParent();
		while( parent != 0 )
		{
			if( parent->GetNodeType() == GAME_OBJECT )
			{
				parentObject = (GameObject*)parent;
				break;
			}
			
			parent = parent->GetParent();
		}
		
		return parentObject;
	}

	GameObject* GameObject::GetRootObject(void)
	{
		GameObject* root = this;
		GameObject* parent = GetParentObject();
		while( parent != 0 )
		{
			root = parent;
			parent = parent->GetParentObject();
		}

		return root;
	}

	IAttribute* GameObject::GetAttribute( AttributeNameKey attributeNameKey )
	{
		map< AttributeNameKey, pair< IAttribute*, unsigned int > >::iterator attribIter = attributes.find( attributeNameKey );

		if( attribIter == attributes.end() )
			return 0;
		
		return attribIter->second.first;
	}

	void GameObject::AddBehavior( BehaviorNameKey behaviorNameKey )
	{
		map< BehaviorNameKey, IBehavior* >::iterator behavIter = behaviors.find( behaviorNameKey );

		if( behavIter != behaviors.end() )
			return;

		IBehavior* newBehavior = ComponentSystem::CreateBehavior( behaviorNameKey );

		if( newBehavior == 0 )
			return;

		newBehavior->gameObject = this;
		//newBehavior->m_GetAttribute = &GameObject::GetAttribute;

		const AttributeNameKey* attribs = newBehavior->GetRequiredAttributes();

		if( attribs != 0 )
			for(; *attribs != 0; ++attribs)
			{
				map< AttributeNameKey, pair< IAttribute*, unsigned int > >::iterator attribIter = attributes.find( *attribs );

				if( attribIter != attributes.end() )
				{
					attribIter->second.second += 1;
					continue;
				}

				IAttribute* newAttribute = ComponentSystem::CreateAttribute( *attribs );
				attributes.insert( pair< AttributeNameKey, pair< IAttribute*, unsigned int > >( *attribs, pair< IAttribute*, unsigned int >( newAttribute, 1 ) ) );
			}

		behaviors.insert( pair< BehaviorNameKey, IBehavior* >( behaviorNameKey, newBehavior ) );


		const MessageMap& behaviorMessageMap = newBehavior->GetMessageMap();
		MessageMap::const_iterator messageMapIter = behaviorMessageMap.begin();
		for(; messageMapIter != behaviorMessageMap.end(); ++messageMapIter )
		{
			const MessageNameKey& msgNameKey = (*messageMapIter).first;

			map< MessageNameKey, set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) > >::iterator msgToBhvrIter = messageToBehaviorMap.find(msgNameKey);

			if( msgToBhvrIter == messageToBehaviorMap.end() )
				msgToBhvrIter = messageToBehaviorMap.insert( pair< MessageNameKey, set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) > >( msgNameKey, set< IBehavior*,bool(*)(const IBehavior*, const IBehavior*) >(IBehavior::comparePriority) ) ).first;
			
			(*msgToBhvrIter).second.insert( newBehavior );
		}

		newBehavior->OnAdd();

		if( visLeaf == 0 )
		{
			MessageT<unsigned int> getRenderableCountMsg( MSG_GET_RENDERABLE_COUNT, 0 );
			OnMessage( &getRenderableCountMsg );
			
			if( getRenderableCountMsg.value != 0 )
				scene->SetObjectRenderableState(this, true);
		}
	}
	
	void GameObject::BuildLocalAabb(void)
	{
		localAabb.min = Float3( FLT_MAX, FLT_MAX, FLT_MAX );
		localAabb.max = Float3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		this->OnMessage( &MessageT<Aabb*>( MSG_BUILD_LOCAL_AABB, &localAabb ) );

		if( localAabb.min.x == FLT_MAX )
		{
			localAabb.min = Float3( -1.0f, -1.0f, -1.0f );
			localAabb.max = Float3( 1.0f, 1.0f, 1.0f );
		}

		gameObjectFlags |= AABB_INVALID;

		if( (gameObjectFlags & STATIC) )
		{
			if( dynAabbTreeLeaf != 0 )
				dynAabbTreeLeaf->tree->Update( dynAabbTreeLeaf, GetAabb() );

			if( visLeaf != 0 )
				visLeaf->tree->Update( visLeaf, GetAabb() );
		}
	}

	void GameObject::RemoveBehavior( BehaviorNameKey behaviorNameKey )
	{
		map< BehaviorNameKey, IBehavior* >::iterator behavIter = behaviors.find( behaviorNameKey );

		if( behavIter == behaviors.end() )
			return;
		
		behavIter->second->OnRemove();

		const AttributeNameKey* attribs = (*behavIter).second->GetRequiredAttributes();

		if( attribs != 0 )
			for(; *attribs != 0; ++attribs)
			{
				map< AttributeNameKey, pair< IAttribute*, unsigned int > >::iterator attribIter = attributes.find( *attribs );

				if( attribIter != attributes.end() )
				{
					attribIter->second.second -= 1;

					if( attribIter->second.second == 0 )
					{
						delete attribIter->second.first;
						attributes.erase( *attribs );
					}
				}
			}

		const MessageMap& behaviorMessageMap = behavIter->second->GetMessageMap();
		MessageMap::const_iterator messageMapIter = behaviorMessageMap.begin();
		for(; messageMapIter != behaviorMessageMap.end(); ++messageMapIter )
		{
			const MessageNameKey& msgNameKey = (*messageMapIter).first;

			map< MessageNameKey, set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) > >::iterator msgToBhvrIter = messageToBehaviorMap.find(msgNameKey);

			if( msgToBhvrIter != messageToBehaviorMap.end() )
			{
				(*msgToBhvrIter).second.erase( behavIter->second );
				
				if( (*msgToBhvrIter).second.empty() )
					messageToBehaviorMap.erase( msgToBhvrIter );
			}	
		}

		delete behavIter->second;
		behaviors.erase( behaviorNameKey );

		if( visLeaf != 0 )
		{
			MessageT<unsigned int> getRenderableCountMsg( MSG_GET_RENDERABLE_COUNT, 0 );
			OnMessage( &getRenderableCountMsg );
			
			if( getRenderableCountMsg.value == 0 )
				scene->SetObjectRenderableState(this, false);
		}
	}

	void GameObject::OnMessage( IMessage* msg )
	{
		if( GetGameObjectFlags() & DELETING )
			return;

		map< MessageNameKey, set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) > >::iterator msgToBhvrIter = messageToBehaviorMap.find(msg->GetNameKey());
		
		if( msgToBhvrIter != messageToBehaviorMap.end() )
		{
			set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) >::iterator behavIter = (*msgToBhvrIter).second.begin();
			set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) >::iterator behavIterEnd = (*msgToBhvrIter).second.end();
			for(; behavIter != behavIterEnd; ++behavIter)
				(*behavIter)->OnMessage( msg );
		}
	}

	void GameObject::OnBroadcastMessage( IMessage* msg )
	{
		OnMessage( msg );
		
		map< GameObject*, TransformKey >::iterator childIter = childObjects.begin();
		for(; childIter != childObjects.end(); ++childIter)
			childIter->first->OnBroadcastMessage( msg );
	}

	void GameObject::OnSendMessageUp( IMessage* msg )
	{
		OnMessage( msg );

		GameObject* parent = GetParentObject();

		if( parent )
			parent->OnSendMessageUp( msg );
	}
}