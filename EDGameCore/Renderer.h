#pragma once

#include "Component.h"
#include "GameObject.h"
#include "../EDMath/aabb.h"
#include "../EDSpatial/DynAabbTree.h"

namespace EDGameCore
{
	static const int MAX_LIGHTS = 8;

	class ILight;
	class IPointLight;

	class IRenderer : public Component
	{
		friend class Game;
		friend class GameObject;
		friend class Rendering;

	public:
		IRenderer(void);
		IRenderer(const IRenderer& rhs)
		{
			renderableTreeLeaf = 0;
			containingTree = 0;
			visibleSetNext = 0;
			memset( lightInfluences, 0, sizeof(lightInfluences) );
		}

		virtual ~IRenderer();

		ComponentType GetComponentType(void)const{ return RENDERER; }

		virtual const EDMath::Aabb& GetAabb(void) = 0;
		virtual void Render(void) = 0;
		virtual bool CastsShadows(void)const = 0;

		virtual unsigned int GetNumLODs(void)const{ return 0; }
		virtual void SetCurrentLOD(unsigned int lod){}
		virtual void SetZDepth(float z){}
	
		IRenderer* clone(void)const = 0;

		static EDMath::Aabb GetAabb(IRenderer& renderer){ return renderer.GetAabb(); }

		IRenderer *GetNextInVisibleSet() { return visibleSetNext; } 

		struct LightInfluence
		{
			float localIntensity;
			ILight* light;
		};
		const LightInfluence *GetLightInfluences(void){ return lightInfluences; }

	private:
		EDSpatial::DynAabbTreeT<IRenderer*>* containingTree;
		EDSpatial::DynAabbTree::LeafT<IRenderer*>* renderableTreeLeaf;

		LightInfluence lightInfluences[MAX_LIGHTS];

		void ClearLights(void);

		void AddLight(ILight* light);
	
		IRenderer* visibleSetNext;
	};

	template<typename Derived>
	class Renderer : public IRenderer
	{
		friend class Game;
		friend class Registry;

	public:
		virtual ~Renderer(void){}

		void LoadState(TiXmlElement* xmlElement){}

		IRenderer* clone(void)const
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
	RegistrationId Renderer<Derived>::typeId = 0;
	
	template<typename Derived>
	string Renderer<Derived>::typeName;
}