#include "precompiled.h"
#include "Scene.h"
#include "DynAabbTreeNode.h"

#include "MessageT.h"

#include "../EDRendererD3D/DebugRenderer.h"

using namespace std;
using namespace GameObjectMessages;
using namespace EDUtilities;
using namespace EDMath;
using namespace EDCollision;

namespace EDGOCA
{
	GameObject* Scene::SpawnObject(const char* xmlFileNamePtr, const Float4x4* xform)
	{
		ContentHandle<GameObjectTemplate> objHandle = ContentManager::LoadXML<GameObjectTemplate>(xmlFileNamePtr);

		GameObjectTemplate* objTemplate = objHandle.GetContent();

		assert( objTemplate != 0 );

		GameObject* pObject = 0;

		bool isUnique = ( (objTemplate->GetFlags() & GameObject::UNIQUE) == GameObject::UNIQUE );

		if( isUnique )
		{
			map<unsigned int, GameObject*>::iterator uniqueIter = uniques.find( objTemplate->GetID() );
			if( uniqueIter != uniques.end() )
				return uniqueIter->second;
		}

		pObject = new GameObject();

		if( xform != 0 )
			pObject->SetLocalTransform(*xform);

		pObject->gameObjectTemplateHandle = objHandle;
		pObject->localAabb = objHandle.GetContent()->GetAabb();
		pObject->gameObjectFlags = objTemplate->GetFlags() | GameObject::AABB_INVALID;
		pObject->scene = this;

		set< BehaviorNameKey >::iterator behavIter = objTemplate->behaviors.begin();
		for(; behavIter != objTemplate->behaviors.end(); ++behavIter )
			pObject->AddBehavior( (*behavIter) );

		map< AttributeNameKey, IAttribute* >::iterator attribIter = objTemplate->attributes.begin();
		for(; attribIter != objTemplate->attributes.end(); ++attribIter )
		{
			map<AttributeNameKey, pair<IAttribute*, unsigned int>>::iterator objAttribIter = pObject->attributes.find( attribIter->first );

			if( objAttribIter != pObject->attributes.end() )
			{
				objAttribIter->second.first->copy( attribIter->second );
				pObject->OnMessage( &MessageT<AttributeNameKey>( MSG_ATTRIBUTE_CHANGED, attribIter->first ) );
			}
		}

		list< TransformData >::iterator xformIter = objTemplate->transforms.begin();
		for(; xformIter != objTemplate->transforms.end(); ++xformIter)
		{
			map< TransformKey, TransformNode* >::iterator setIter = pObject->transformSet.find( xformIter->transformKey );
			if( setIter == pObject->transformSet.end() )
			{
				TransformNode* node = new TransformNode();
				node->SetTransformInheritence(xformIter->transformFlag);
				node->SetLocalTransform( xformIter->localMatrix );
				pObject->AddChild( node );
				pObject->transformSet.insert( pair<TransformKey, TransformNode*>( xformIter->transformKey, node ) );
			}
			else
				setIter->second->SetLocalTransform( xformIter->localMatrix );
		}

		pObject->dynAabbTreeLeaf = new DynAabbTreeLeafNode(pObject);

		if( isUnique )
			uniques.insert( pair<unsigned int, GameObject*>( objTemplate->GetID(), pObject ) );

		if( (pObject->GetGameObjectFlags() & GameObject::UPDATEABLE) == GameObject::UPDATEABLE )
			updateables.push_back( pObject );

		if( (pObject->GetGameObjectFlags() & GameObject::STATIC) == GameObject::STATIC )
			staticObjectTree.Insert( pObject->dynAabbTreeLeaf );
		else
			dynamicObjectTree.Insert( pObject->dynAabbTreeLeaf );

		pObject->DirtyNode();

		masterObjectSet.insert( pObject );

		list< pair<string, TransformKey> >::iterator childIter = objTemplate->childObjects.begin();
		for(; childIter != objTemplate->childObjects.end(); ++childIter)
		{
			GameObject* child = SpawnObject(childIter->first.c_str());
			if( child != 0 )
				pObject->AddChildObject( child, childIter->second );
		}

		return pObject;
	}

	void Scene::DespawnObject(GameObject* object)
	{
		if( object->GetGameObjectFlags() & GameObject::DELETING )
			return;
		
		object->gameObjectFlags |= GameObject::DELETING;
		deletingObjects.push_back( object );
	}

	GameObject* Scene::GetUniqueObject(unsigned int id)
	{
		map<unsigned int, GameObject*>::iterator uniqueIter = uniques.find( id );
		if( uniqueIter != uniques.end() )
			return uniqueIter->second;

		return 0;
	}

	void Scene::SetObjectRenderableState(GameObject* object, bool state)
	{
		if( object->scene != this )
			return;

		if( object->visLeaf == 0 && state == false )
			return;

		if( object->visLeaf != 0 && state == true )
			return;


		if( state == true )
		{
			object->visLeaf = new DynAabbTreeLeafNode(object);
			visTree.Insert( object->visLeaf );
		}
		else
		{
			visTree.Remove( object->visLeaf );
			delete object->visLeaf;
			object->visLeaf = 0;
		}
	}

	void Scene::Update(void)
	{
		while( !deletingObjects.empty() )
		{
			GameObject* pObject = deletingObjects.front();
			if( pObject->GetGameObjectFlags() & GameObject::UPDATEABLE )
				updateables.remove( pObject );
			
			if( pObject->GetGameObjectFlags() & GameObject::UNIQUE )
				uniques.erase( pObject->gameObjectTemplateHandle.GetContent()->GetID() );

			if( (pObject->GetGameObjectFlags() & GameObject::STATIC) || (pObject->GetGameObjectFlags() & GameObject::ASLEEP) )
				staticObjectTree.Remove( pObject->dynAabbTreeLeaf );
			else
				dynamicObjectTree.Remove( pObject->dynAabbTreeLeaf );

			if( pObject->visLeaf != 0 )
			{
				visTree.Remove( pObject->visLeaf );
				delete pObject->visLeaf;
				pObject->visLeaf = 0;
			}

			delete pObject->dynAabbTreeLeaf;
			pObject->dynAabbTreeLeaf = 0;

			masterObjectSet.erase( pObject );

			delete pObject;

			deletingObjects.pop_front();
		}

		list<GameObject*>::iterator updateIter = updateables.begin();
		for(; updateIter != updateables.end(); ++updateIter)
		{
			(*updateIter)->Update();

			const Aabb& objAABB = (*updateIter)->GetAabb();
			const Aabb& leafAABB = (*updateIter)->dynAabbTreeLeaf->GetAABB();

			if( (*updateIter)->GetGameObjectFlags() & GameObject::ASLEEP )
			{
				if( (*updateIter)->GetGameObjectFlags() & GameObject::MOVED )
				{
					staticObjectTree.Remove( (*updateIter)->dynAabbTreeLeaf );
					dynamicObjectTree.Insert( (*updateIter)->dynAabbTreeLeaf );
					(*updateIter)->gameObjectFlags &= ~GameObject::ASLEEP;
				}
				else
					continue;
			}

			dynamicObjectTree.Update( (*updateIter)->dynAabbTreeLeaf, (*updateIter)->GetAabb() );

			if( (*updateIter)->visLeaf != 0 )
				visTree.Update( (*updateIter)->visLeaf, (*updateIter)->GetAabb() );

			if( ((*updateIter)->GetGameObjectFlags() & GameObject::MOVED) == 0 )
			{
				dynamicObjectTree.Remove( (*updateIter)->dynAabbTreeLeaf );
				staticObjectTree.Insert( (*updateIter)->dynAabbTreeLeaf );
				(*updateIter)->gameObjectFlags |= GameObject::ASLEEP;
			}

			(*updateIter)->gameObjectFlags &= ~GameObject::MOVED;
		}

		DynAabbTree::TreeCollision( &dynamicObjectTree, &dynamicObjectTree );
		DynAabbTree::TreeCollision( &dynamicObjectTree, &staticObjectTree );
	}

	Scene::~Scene(void)
	{
		while( !masterObjectSet.empty() )
		{
			GameObject* pObject = *(masterObjectSet.begin());
			if( pObject->GetGameObjectFlags() & GameObject::UPDATEABLE )
				updateables.remove( pObject );
			
			if( pObject->GetGameObjectFlags() & GameObject::UNIQUE )
				uniques.erase( pObject->gameObjectTemplateHandle.GetContent()->GetID() );

			if( (pObject->GetGameObjectFlags() & GameObject::STATIC) || (pObject->GetGameObjectFlags() & GameObject::ASLEEP) )
				staticObjectTree.Remove( pObject->dynAabbTreeLeaf );
			else
				dynamicObjectTree.Remove( pObject->dynAabbTreeLeaf );

			if( pObject->visLeaf != 0 )
			{
				visTree.Remove( pObject->visLeaf );
				delete pObject->visLeaf;
				pObject->visLeaf = 0;
			}

			delete pObject->dynAabbTreeLeaf;
			pObject->dynAabbTreeLeaf = 0;

			masterObjectSet.erase( pObject );

			delete pObject;
		}
	}

	//void Scene::FrustumCull( FrustumHull* frustum, IMessage* msg )
	//{
	//	if( visTree.GetOcclusionQueryCreator() != 0 && ((sceneFlags & SF_OCCLUSION_CULL) == SF_OCCLUSION_CULL) )
	//		DynAabbTree::FrustumAndOcclusionCull( &visTree, frustum, msg );
	//	else
	//		DynAabbTree::FrustumCull( &visTree, frustum, msg );
	//}

	void Scene::SetOcclusionQueryCreator(IOcclusionQueryCreator creator)
	{
		visTree.SetOcclusionQueryCreator(creator);
	}

	//void Scene::ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const Segment& segment, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &staticObjectTree, perObjFunc, segment, voidPtr );
	//	DynAabbTree::ForAllOverlappedObjects( &dynamicObjectTree, perObjFunc, segment, voidPtr );
	//}

	//void Scene::ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &staticObjectTree, perObjFunc, sphere, voidPtr );
	//	DynAabbTree::ForAllOverlappedObjects( &dynamicObjectTree, perObjFunc, sphere, voidPtr );
	//}

	//void Scene::ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const Aabb& aabb, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &staticObjectTree, perObjFunc, aabb, voidPtr );
	//	DynAabbTree::ForAllOverlappedObjects( &dynamicObjectTree, perObjFunc, aabb, voidPtr );
	//}

	//void Scene::ForAllOverlappedCollidableObjects( PerOverlappedObjectFunc perObjFunc, const FrustumHull& frustum, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &staticObjectTree, perObjFunc, frustum, voidPtr );
	//	DynAabbTree::ForAllOverlappedObjects( &dynamicObjectTree, perObjFunc, frustum, voidPtr );
	//}

	//void Scene::ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const Segment& segment, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &visTree, perObjFunc, segment, voidPtr );
	//}

	//void Scene::ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const EDCollision::Sphere& sphere, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &visTree, perObjFunc, sphere, voidPtr );
	//}

	//void Scene::ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const Aabb& aabb, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &visTree, perObjFunc, aabb, voidPtr );
	//}

	//void Scene::ForAllOverlappedRenderableObjects( PerOverlappedObjectFunc perObjFunc, const FrustumHull& frustum, void* voidPtr )
	//{
	//	DynAabbTree::ForAllOverlappedObjects( &visTree, perObjFunc, frustum, voidPtr );
	//}
}