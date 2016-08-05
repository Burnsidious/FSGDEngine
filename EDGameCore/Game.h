#pragma once

#include "../EDUtilities/timer.h"
#include "Rendering.h"
#include "Physics.h"
#include "LayerManager.h"
#include "Camera.h"
#include "Light.h"
#include "SkyBox.h"
#include "Registry.h"
#include <list>
#include <map>
#include <set>
#include <unordered_map>
using namespace std;

namespace EDGameCore
{
	struct RayHit;

	class Game : public Rendering, public Physics, public Registry, public LayerManager
	{
		friend class GameObject;
		class VisibleTreeLeaf;
	public:
		Game(void);
		
		virtual ~Game(void){}

		virtual void Initialize(void);
	
		virtual void Shutdown(void);

		virtual void Update(void);

		static GameObject* GetGameObjectInstance(unsigned int instanceId)
		{
			return GameObject::GetGameObjectInstance(instanceId);
		}

		static void FindObjectsByTag(const char* tag, list<GameObject*>& objList);

		static TiXmlElement* LoadDoc(TiXmlDocument& doc, const char* fileName);

		static float GetDeltaTime(void){ return deltaTime; }


		static GameObject* Instantiate(RegistrationId prefabId);
		static GameObject* Instantiate(RegistrationId prefabId, const Float4x4& xform);
		static GameObject* Instantiate(const char* prefabName);
		static GameObject* Instantiate(const char* prefabName, const Float4x4& xform);

		static void LoadScene(const char* _scene_name);

	protected:

		static void LoadGame(const char* fileName);

	private:

		static list<string> loadingSceneList;
		static list<GameObject*> wakingList;

		EDUtilities::Timer timer;
		static float deltaTime;
		float elapsedTime;
		static TiXmlDocument gameDoc;
		static TiXmlElement* gameElement;
		static std::string gameFileName;

		virtual void RegisterTypes(void){}
		virtual void RegisterStrings(void){}

		static void _loadScene(const char* fileName);

		void BeginUpdate(void);

		void FixedUpdate(void);

		static void GameObjectUpdate(void);
		static void GameObjectLateUpdate(void);

		//Prefab Management
		static unordered_map< RegistrationId, GameObject* > prefabMap;
		static void LoadPrefabs(TiXmlElement* xmlElement);

		static void LoadLayers(TiXmlElement* xmlElement);

		static void LoadPhysicsMaterials(TiXmlElement* xmlElement);

		static GameObject* _Instantiate(RegistrationId prefabId);
		static GameObject* _Instantiate(RegistrationId prefabId, const Float4x4& xform);
		static GameObject* _Instantiate(const char* prefabName);
		static GameObject* _Instantiate(const char* prefabName, const Float4x4& xform);

	};

#define STRINGIFY(x) #x

#define RegisterType(x) RegisterTypeT<x>( STRINGIFY(x) )
}