#pragma once

#include "Behavior.h"
#include "Transform.h"
#include "../EDCollision/aabb.h"
#include "../EDCollision/sphere.h"
#include "../EDSpatial/DynAabbTree.h"
#include <list>
using namespace std;

namespace EDGameCore
{
	class IRenderer;
	
	class ILight : public Component
	{
		//friend class Game;
		friend class Rendering;
		friend class GameObject;

	public:
		enum LightType{ POINT = 0, SPOT, DIRECTIONAL };
		enum LightShadows{ NONE = 0, SOFT };

		ILight(void)
		{
			cullMask = 0xFFFFFFFF;
			color.x = color.y = color.z = 1.0f;
			intensity = 1.0f;
			visibleSetHead = 0;
			visibleSetNext = 0;
			shadows = NONE;
		}

		ILight(const ILight& rhs)
		{
			cullMask = rhs.cullMask;
			color = rhs.color;
			intensity = rhs.intensity;
			visibleSetHead = 0;
			visibleSetNext = 0;
			shadows = rhs.shadows;
		}

		virtual ~ILight(){}

		ComponentType GetComponentType(void)const{ return LIGHT; }

		virtual LightType GetLightType(void) = 0;
		virtual const EDMath::Aabb& GetAabb(void) = 0;
		virtual bool TestAabb(const EDMath::Aabb& aabb) = 0;

		unsigned int GetCullMask(void){ return cullMask; }
		void SetCullMask(unsigned int m){ cullMask = m; }
		
		LightShadows GetShadows(void){ return shadows; }
		void SetShadows(LightShadows ls){ shadows = ls; }

		Float3 GetColor(void){ return color; }
		void SetColor(Float3 c){ color = c; }
		float GetIntensity(void){ return intensity; }
		void SetIntensity(float i){ intensity = i; }

		virtual void Update(void) = 0;
		virtual void Render(void){}

		ILight* clone(void)const = 0;

		template<typename Func>
		static void ForEachRendererInLight(ILight *light, Func func)
		{
			for(IRenderer *current = light->visibleSetHead; 
				current != nullptr; current = current->GetNextInVisibleSet())
				func(current);
		}

		template<typename Func>
		void ForEachRendererInLight(Func func)
		{
			for(IRenderer *current = visibleSetHead; current != nullptr; 
				current = current->GetNextInVisibleSet())
				func(current);
		}

		virtual bool CullRenderable(IRenderer* renderable) = 0;

	protected:

		unsigned int cullMask;
		Float3 color;
		float intensity;
		LightShadows shadows;

		IRenderer* visibleSetHead;
		ILight* visibleSetNext;
	};

	class IPointLight : public ILight
	{
		friend class Game;
		friend class GameObject;
		friend class Rendering;
	public:
		IPointLight(void) : lightTreeLeaf(0), containingTree(0), range(1.0f) {}
		
		IPointLight(const IPointLight& rhs) : ILight(rhs)
		{
			lightTreeLeaf = 0;
			containingTree = 0;
			
			range = rhs.range;
			worldAabb = rhs.worldAabb;
		}

		virtual ~IPointLight();

		LightType GetLightType(void){ return POINT; }

		void Update(void);
		bool CullRenderable(IRenderer* renderable);

		float GetRange(void){ return range; }
		virtual void SetRange(float r){ range = r; }

		bool TestAabb(const EDMath::Aabb& aabb);

		const EDMath::Aabb& GetAabb(void);

		IPointLight* clone(void)const = 0;

		static EDMath::Aabb GetAabb(IPointLight& light){ return light.GetAabb(); }

	protected:
		float range;
		EDMath::Aabb worldAabb;

		EDSpatial::DynAabbTreeT<IPointLight*>* containingTree;
		EDSpatial::DynAabbTree::LeafT<IPointLight*>* lightTreeLeaf;
	};

	class IDirectionalLight : public ILight
	{
		friend class Game;
		friend class GameObject;
		friend class Rendering;
	public:
		IDirectionalLight(void) : containingList(0) {}
		IDirectionalLight(const IDirectionalLight& rhs) : ILight(rhs)
		{
			containingList = 0;
			direction = rhs.direction;
			aabb = rhs.aabb;
		}

		virtual ~IDirectionalLight();

		LightType GetLightType(void){ return DIRECTIONAL; }

		void Update(void);
		bool CullRenderable(IRenderer* renderable);

		void SetAabb(const EDMath::Aabb& b){ aabb = b; }
		const EDMath::Aabb& GetAabb(void){ return aabb; }
		bool TestAabb(const EDMath::Aabb& aabb);

		IDirectionalLight* clone(void)const = 0;

	private:
		Float3 direction;
		EDMath::Aabb aabb;

		list<IDirectionalLight*>* containingList;
	};

	class ISpotLight : public IPointLight
	{
		friend class Game;
		friend class GameObject;
	public:
		ISpotLight(void) : spotAngle(0.785398f), radius(1.0f) {}
		virtual ~ISpotLight(){}

		LightType GetLightType(void){ return SPOT; }
		float GetSpotAngle(void){ return spotAngle; }
		void SetSpotAngle(float s);
		void SetRange(float r);

		bool TestAabb(const EDMath::Aabb& aabb);

		const EDMath::Aabb& GetAabb(void);

		ISpotLight* clone(void)const = 0;

	protected:
		float spotAngle;
		float radius;
	};

	template<typename Derived>
	class PointLight : public IPointLight
	{
		friend class Game;
		friend class Registry;
	public:
		virtual ~PointLight(){}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }

		void LoadState(TiXmlElement* xmlElement){}

	private:
		IPointLight* clone(void)const
		{
			return new Derived(*(Derived*)this);
		}

		static RegistrationId typeId;
		static string typeName;
	};
	
	template<typename Derived>
	RegistrationId PointLight<Derived>::typeId = 0;
	
	template<typename Derived>
	string PointLight<Derived>::typeName;

	template<typename Derived>
	class SpotLight : public ISpotLight
	{
		friend class Game;
		friend class Registry;

	public:
		virtual ~SpotLight(){}

		void LoadState(TiXmlElement* xmlElement){}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }

	private:
		ISpotLight* clone(void)const
		{
			return new Derived(*(Derived*)this);
		}

		static RegistrationId typeId;
		static string typeName;
	};

	template<typename Derived>
	RegistrationId SpotLight<Derived>::typeId = 0;
	
	template<typename Derived>
	string SpotLight<Derived>::typeName;

	template<typename Derived>
	class DirectionalLight : public IDirectionalLight
	{
		friend class Game;
		friend class Registry;

	public:
		virtual ~DirectionalLight(){}
	
		void LoadState(TiXmlElement* xmlElement){}

		RegistrationId GetTypeId(void){ return typeId; }
		const char* GetTypeName(void){ return typeName.empty() ? 0 : typeName.c_str(); }

	private:
		IDirectionalLight* clone(void)const
		{
			return new Derived(*(Derived*)this);
		}

		static RegistrationId typeId;
		static string typeName;
	};

	template<typename Derived>
	RegistrationId DirectionalLight<Derived>::typeId = 0;
	
	template<typename Derived>
	string DirectionalLight<Derived>::typeName;
}