#include "Rendering.h"
#include "Game.h"
#include <assert.h>
#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "WheelCollider.h"
#include "MeshCollider.h"
#include "CharacterCollider.h"

#include "RigidBody.h"
#include "Camera.h"
#include "../EDCollision/sphere.h"
#include "../EDCollision/capsule.h"
#include "../EDCollision/segment.h"
#include "../EDCollision/ray.h"
#include "../EDCollision/aabb.h"
#include "Light.h"
#include "Animation.h"
#include "../tinyxml/tinyxml.h"
#include "LayerManager.h"
#include "Physics.h"

#include "../EDUtilities/ContentManager.h"
#include "../EDPhysics/RigidBody.h"
#include "SkyBox.h"

using namespace EDUtilities;

namespace EDGameCore
{
	list<string> Game::loadingSceneList;
	list<GameObject*> Game::wakingList;

	unordered_map< RegistrationId, GameObject* > Game::prefabMap;
	float Game::deltaTime = 0.0f;
	TiXmlDocument Game::gameDoc;
	TiXmlElement* Game::gameElement = 0;
	std::string Game::gameFileName;

	Game::Game(void)
	{
		elapsedTime = 0.0f;
		timer.Reset();
	}

	void Game::Initialize(void)
	{
		RegisterType(Transform);
		RegisterType(RigidBody);
		RegisterType(SphereCollider);
		RegisterType(CapsuleCollider);
		RegisterType(BoxCollider);
		RegisterType(BVHCollider);
		RegisterType(Camera);
		RegisterType(WheelCollider);
		RegisterType(GameObject);
		RegisterType(Animation);
		RegisterType(CharacterCollider);

		RegisterTypes();

		RegisterStrings();
		timer.Reset();
	}

	void Game::Shutdown(void)
	{
		auto func = [](GameObject* obj)
		{
			destroy(obj);
		};

		GameObject::ForEachGameObject( func );

		auto iter = prefabMap.begin();
		for(; iter != prefabMap.end(); ++iter)
			destroy( iter->second );

		if( skyBoxPtr != 0 )
			destroy(skyBoxPtr);

		CleanUp();
	}

	void Game::Update(void)
	{
		// Start the new update, updating or resetting any per-update values
		BeginUpdate();

		// Perform the fixed update step.
		// Processes physics and collisions detection
		// in a framerate independent manner.
		FixedUpdate();
		
		GameObjectUpdate();

		GameObjectLateUpdate();

		// Delete destroyed objects
		CleanUp();
	}

	void Game::BeginUpdate(void)
	{
		Rendering::BeginFrame();

		while( !loadingSceneList.empty() )
		{
			_loadScene( loadingSceneList.begin()->c_str() );
			loadingSceneList.pop_front();

			CleanUp();

			while( !wakingList.empty() )
			{
				GameObject* gameObject = wakingList.front();
				wakingList.pop_front();

				Transform::ForEachTransformInHierarchy( gameObject->GetTransform(), [](Transform* t){ t->GetGameObject()->Awake(); } );
			}

			elapsedTime = 0.0f;
			timer.Reset();
		}
		

		deltaTime = timer.GetElapsedTime();
		timer.Reset();

		elapsedTime += deltaTime;
	}

	void Game::FixedUpdate(void)
	{
		// Cache the real delta time
		float realDeltaTime = deltaTime;

		// Set delta time to fixed time-step amount
		// for physics simulation and FixedUpdates
		deltaTime = EDPhysics::TIME_STEP;

		// Semi-fixed time-step with leftover time preservation.
		// Elapsed time is broken into discrete EDPhysics::TIME_STEP size chunks.
		// Any left over elapsed time is preserved and added to the elapsed time of the next frame.
		while( elapsedTime >= EDPhysics::TIME_STEP )
		{
			// For each GameObject, perform FixedUpdate operations on components and behaviors
			GameObject::ForEachGameObject( [](GameObject* obj){ Physics::FixedUpdate(obj); } );

			// Determine all collisions and process reactions
			Physics::ProcessCollisions();

			elapsedTime -= EDPhysics::TIME_STEP;
		}

		// Restore the real delta time
		deltaTime = realDeltaTime;
	}

	void Game::FindObjectsByTag(const char* tag, list<GameObject*>& objList)
	{
		auto func = 
		[&](GameObject* obj)
		{
			if(obj->HasTag(tag) && !obj->isDestroying())
				objList.push_back(obj);
		};

		GameObject::ForEachGameObject(func);
	}

	TiXmlElement* Game::LoadDoc(TiXmlDocument& doc, const char* fileName)
	{
		string temp = ContentManager::theContentPath;
		temp += string(fileName);
		const char* xmlFileNamePtr = temp.c_str();
		
		if(!doc.LoadFile(xmlFileNamePtr))
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

		return elementPtr;
	}

	TiXmlElement* GetRootGame(TiXmlDocument& doc, const char* fileName)
	{
		TiXmlElement* elementPtr = Game::LoadDoc(doc, fileName);

		if( elementPtr != 0 )
		{
			std::string rootName = elementPtr->Value();

			if( strcmp(rootName.c_str(), "Game") != 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing a Game element\n", 
					fileName);
				return 0;
			}
		}

		return elementPtr;
	}

	void Game::LoadGame(const char* fileName)
	{
		gameElement = GetRootGame(gameDoc, fileName);
		
		if( gameElement == 0 )
			return;

		gameFileName = fileName;

		TiXmlElement* layerEle = gameElement->FirstChildElement("Layers");
		if( layerEle != 0 )
			LoadLayers(layerEle);

		EDPhysics::AddPhysicsMaterial( EDPhysics::MATERIAL_DEFAULT, EDPhysics::PhysicsMaterial() );
		TiXmlElement* physMatEle = gameElement->FirstChildElement("PhysicsMaterials");
		if( physMatEle != 0 )
			LoadPhysicsMaterials( physMatEle );

		TiXmlElement* prefabEle = gameElement->FirstChildElement("Prefabs");

		if( prefabEle != 0 )
			LoadPrefabs(prefabEle);

		TiXmlElement* elementPtr = gameElement->FirstChildElement("Scene");
	
		if( elementPtr != 0 )
		{
			TiXmlElement* sceneNameEle = elementPtr->FirstChildElement("Name");

			if( sceneNameEle != 0 &&
				sceneNameEle->GetText() != 0 &&
				strlen( sceneNameEle->GetText() ) != 0 )
			{
				LoadScene( sceneNameEle->GetText() );
			}			
		}
	}

	void Game::LoadScene(const char* _scene_name)
	{
		bool valid_scene = false;

		TiXmlElement* elementPtr = gameElement->FirstChildElement("Scene");
		for(; elementPtr != 0; elementPtr = elementPtr->NextSiblingElement("Scene") )
		{
			TiXmlElement* sceneNameEle = elementPtr->FirstChildElement("Name");

			if( sceneNameEle != 0 && sceneNameEle->GetText() != 0 && 
				strcmp( sceneNameEle->GetText(), _scene_name ) == 0 )
			{
					valid_scene = true;
					break;
			}
		}

		if (valid_scene)
		{
			loadingSceneList.push_back( string(_scene_name) );
		}
		else
		{
			InternalOutput::GetReference().Error("Failed to load scene:  %s\n", _scene_name);
		}
	}

	void Game::_loadScene(const char* sceneName)
	{
		auto func = 
		[](GameObject* obj)
		{
			GameObject* currentObj = obj;
			
			while(currentObj != 0)
			{
				if( !currentObj->DestroysOnLoad() )
					return;

				Transform* parent = currentObj->GetTransform()->GetParent();

				if( parent == 0 )
					break;

				currentObj = parent->GetGameObject();
			}

			destroy( obj );
		};

		GameObject::ForEachGameObject( func );

		if( skyBoxPtr != 0 )
		{
			destroy(skyBoxPtr);
			skyBoxPtr = 0;
		}

		wakingList.clear();

		if( gameElement == 0 )
			return;

		TiXmlElement* elementPtr = gameElement->FirstChildElement("Scene");
		
		for(; elementPtr != 0; elementPtr = elementPtr->NextSiblingElement("Scene") )
		{
			TiXmlElement* sceneNameEle = elementPtr->FirstChildElement("Name");

			if( sceneNameEle != 0 &&
				sceneNameEle->GetText() != 0 &&
				strlen( sceneNameEle->GetText() ) != 0 &&
				strcmp( sceneNameEle->GetText(), sceneName ) == 0 )
				break;
		}
		
		if( elementPtr == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s. Scene not found in %s\n", 
				sceneName, gameFileName.c_str());
			return;
		}

		TiXmlElement* skyboxEle = elementPtr->FirstChildElement("SkyBox");
		if( skyboxEle != 0 )
		{
			TiXmlElement* typeEle = skyboxEle->FirstChildElement("TypeName");

			if( typeEle != 0 &&
				typeEle->GetText() != 0 &&
				strlen( typeEle->GetText() ) != 0 )
			{
				Object* result = create( typeEle->GetText() );

				skyBoxPtr = (ISkyBox*)result;

				if( skyBoxPtr != 0 )
					skyBoxPtr->LoadState( skyboxEle );
			}
		}

		TiXmlElement* instanceEle = elementPtr->FirstChildElement("Instance");
		
		for(; instanceEle != 0; instanceEle = instanceEle->NextSiblingElement("Instance"))
		{
			GameObject* instancePtr = 0;

			TiXmlElement* prefabEle = instanceEle->FirstChildElement("Prefab");
			if( prefabEle == 0 ||
				prefabEle->GetText() == 0 ||
				strlen(prefabEle->GetText()) == 0 )
				continue; 

			TiXmlElement *instanceXformEle = instanceEle->FirstChildElement("Transform");

			if( instanceXformEle != 0 )
			{
				Float4x4 localMatrix;
				localMatrix.makeIdentity();

				const unsigned int NumAxis = 4;
				const unsigned int NumComponents = 4;
				const char *axes[NumAxis] = {"XAxis", "YAxis", "ZAxis", "WAxis"};
				const char *components[NumComponents] = {"x", "y", "z", "w"};
				TiXmlElement *axisEle[4];

				for(unsigned int axis = 0; axis < NumAxis; ++axis)
				{
					axisEle[axis] = instanceXformEle->FirstChildElement(axes[axis]);

					if( axisEle[axis] == 0 )
						continue;

					for(int component = 0; component < NumComponents; ++component)
					{
						TiXmlElement *compEle = axisEle[axis]->FirstChildElement(components[component]);
				
						if(compEle != 0 &&
							compEle->GetText() != 0 &&
							strlen( compEle->GetText() ) != 0 )
						{
							localMatrix.m[axis][component] = (float)atof(compEle->GetText());
						}
					}
				}
				// Test if only a forward axis was provided, and if so build the others
				if(!axisEle[0] && !axisEle[1] && axisEle[2])
				{
					CrossProduct(localMatrix.XAxis, Float3(0, 1, 0), localMatrix.ZAxis); 
					localMatrix.XAxis.normalize();
					CrossProduct(localMatrix.YAxis, localMatrix.ZAxis, localMatrix.XAxis); 
					localMatrix.YAxis.normalize();
				}

				instancePtr = _Instantiate( prefabEle->GetText(), localMatrix );
			}
			else
				instancePtr = _Instantiate( prefabEle->GetText() );

			if( instancePtr != 0 )
			{
				wakingList.push_back(instancePtr);

				TiXmlElement* nameEle = instanceEle->FirstChildElement("Name");
				
				if( nameEle != 0 &&
					nameEle->GetText() != 0 &&
					strlen(nameEle->GetText()) != 0 )
					instancePtr->SetName( nameEle->GetText() );

				TiXmlElement* rigidBodyEle = instanceEle->FirstChildElement("RigidBody");
				if( rigidBodyEle != 0 && instancePtr->GetRigidBody() != 0 )
					instancePtr->GetRigidBody()->LoadState(rigidBodyEle);
			}
		}
	}

	void Game::LoadLayers(TiXmlElement* xmlElement)
	{
		const unsigned int STD_LAYER_COUNT = 1;
		const char* stdLayers[STD_LAYER_COUNT] = { "Default" };

		unsigned int mask = 0;
		for(; mask < STD_LAYER_COUNT; ++mask)
		{
			RegistrationId layerId = RegisterString( stdLayers[mask] );
			LayerManager::SetLayer( mask, layerId );
		}

		TiXmlElement* layerEle = xmlElement->FirstChildElement("Name");
		
		for(; layerEle != 0 && mask < 32; layerEle = layerEle->NextSiblingElement("Name") )
		{
			if( layerEle->GetText() != 0 &&
				strlen( layerEle->GetText() ) != 0 )
			{
				RegistrationId layerId = RegisterString( layerEle->GetText() );
				
				LayerManager::SetLayer( mask, layerId );

				++mask;
			}
		}

		TiXmlElement* interactEle = xmlElement->FirstChildElement("Interaction");
		for(; interactEle != 0; interactEle = interactEle->NextSiblingElement("Interaction"))
		{
			TiXmlElement* firstEle = interactEle->FirstChildElement("First");

			if( firstEle == 0 ||
				firstEle->GetText() == 0 ||
				strlen(firstEle->GetText()) == 0 )
				continue;

			RegistrationId firstId = GetStringRegistrationId( firstEle->GetText() );
			
			if( firstId == 0 )
				continue;

			if( LayerManager::GetLayerIndex( firstId ) == -1 )
				continue;

			TiXmlElement* secondEle = interactEle->FirstChildElement("Second");

			if( secondEle == 0 ||
				secondEle->GetText() == 0 ||
				strlen(secondEle->GetText()) == 0 )
				continue;
			
			RegistrationId secondId = GetStringRegistrationId( secondEle->GetText() );

			if( secondId == 0 )
				continue;

			if( LayerManager::GetLayerIndex( secondId ) == -1 )
				continue;

			LayerManager::AddLayerInteractionSet( firstId, secondId );
		}
	}

	void Game::LoadPhysicsMaterials(TiXmlElement* xmlElement)
	{
		TiXmlElement* materialEle = xmlElement->FirstChildElement("Material");

		for(; materialEle != 0; materialEle = materialEle->NextSiblingElement("Material") )
		{
			TiXmlElement* nameEle = materialEle->FirstChildElement("Name");

			if( nameEle == 0 ||
				nameEle->GetText() == 0 ||
				strlen(nameEle->GetText()) == 0 )
				continue;

			EDPhysics::PhysicsMaterial material;
			RegistrationId matId = RegisterString( nameEle->GetText() );

			TiXmlElement* fricEle = materialEle->FirstChildElement("Friction");
			if( fricEle != 0 &&
				fricEle->GetText() != 0 &&
				strlen(fricEle->GetText()) != 0 )
				material.friction = (float)atof(fricEle->GetText());

			TiXmlElement* fricEle2 = materialEle->FirstChildElement("Friction2");
			if( fricEle2 != 0 &&
				fricEle2->GetText() != 0 &&
				strlen(fricEle2->GetText()) != 0 )
				material.friction2 = (float)atof(fricEle2->GetText());

			TiXmlElement* bounceEle = materialEle->FirstChildElement("Bounciness");
			if( bounceEle != 0 &&
				bounceEle->GetText() != 0 &&
				strlen(bounceEle->GetText()) != 0 )
				material.bounciness = (float)atof(bounceEle->GetText());

			TiXmlElement* fric2DirEle = materialEle->FirstChildElement("FrictionDirection2");

			if( fric2DirEle != 0 )
			{
				const char* components[] = {"x", "y", "z"};

				for(int j = 0; j < 3; ++j)
				{
					TiXmlElement* compEle = fric2DirEle->FirstChildElement(components[j]);
				
					if(compEle != 0 &&
						compEle->GetText() != 0 &&
						strlen( compEle->GetText() ) != 0 )
					{
						material.frictionDirection2.v[j] = (float)atof(compEle->GetText());
					}
				}
			}

			EDPhysics::AddPhysicsMaterial( matId, material );
		}
	}

	void Game::LoadPrefabs(TiXmlElement* xmlElement)
	{
		TiXmlElement* gameObjEle = xmlElement->FirstChildElement("GameObject");
		
		list<TiXmlElement*> prefabList;

		for(; gameObjEle != 0; gameObjEle = gameObjEle->NextSiblingElement("GameObject") )
			prefabList.push_back( gameObjEle );

		while( !prefabList.empty() )
		{
			gameObjEle = prefabList.front();
			prefabList.pop_front();
			
			GameObject* gameObj = GameObject::Load( gameObjEle );
			
			if( gameObj != 0 )
			{
				RegistrationId prefabId = RegisterString( gameObj->GetName() );
				prefabMap.insert( make_pair(prefabId, gameObj) );
			}
		}
	}

	GameObject* Game::_Instantiate(RegistrationId prefabId)
	{
		auto iter = prefabMap.find(prefabId);
		
		if( iter == prefabMap.end() )
			return 0;

		GameObject* result = iter->second->clone();

		if( result != 0 )
		{
			auto storeFunc =
			[](Transform* t)
			{
				GameObject::gameObjectList.push_back( t->GetGameObject() );
				t->GetGameObject()->enable();
			};

			Transform::ForEachTransformInHierarchy( result->GetTransform(), storeFunc );
		}

		return result;
	}

	GameObject* Game::_Instantiate(RegistrationId prefabId, const Float4x4& xform)
	{
		GameObject* result = _Instantiate( prefabId );

		if( result != 0 )
			result->GetTransform()->SetLocalMatrix( xform );
		
		return result;
	}

	GameObject* Game::_Instantiate(const char* prefabName)
	{
		return _Instantiate( GetStringRegistrationId(prefabName) );
	}

	GameObject* Game::_Instantiate(const char* prefabName, const Float4x4& xform)
	{
		return _Instantiate( GetStringRegistrationId(prefabName), xform );
	}

	GameObject* Game::Instantiate(RegistrationId prefabId)
	{
		GameObject* result = _Instantiate(prefabId);

		if( result != 0 )
		{
			Transform::ForEachTransformInHierarchy( result->GetTransform(), [](Transform* t){ t->GetGameObject()->Awake(); } );
		}

		return result;
	}

	GameObject* Game::Instantiate(RegistrationId prefabId, const Float4x4& xform)
	{
		GameObject* result = Instantiate( prefabId );

		if( result != 0 )
			result->GetTransform()->SetLocalMatrix( xform );
		
		return result;
	}

	GameObject* Game::Instantiate(const char* prefabName)
	{
		return Instantiate( GetStringRegistrationId(prefabName) );
	}

	GameObject* Game::Instantiate(const char* prefabName, const Float4x4& xform)
	{
		return Instantiate( GetStringRegistrationId(prefabName), xform );
	}

	void Game::GameObjectUpdate(void)
	{
		auto iter = GameObject::gameObjectList.begin();
		auto end = GameObject::gameObjectList.end();

		for(; iter != end; ++iter)
		{
			if( (*iter)->isEnabledInHierarchy() )
				(*iter)->Update();
		}
	}

	void Game::GameObjectLateUpdate(void)
	{
		auto iter = GameObject::gameObjectList.begin();
		auto end = GameObject::gameObjectList.end();

		for(; iter != end; ++iter)
		{
			if( (*iter)->isEnabledInHierarchy() )
			{
				(*iter)->LateUpdate();
				
				Physics::Update( (*iter) );
				Rendering::Update( (*iter) );
			}
		}	
	}
}