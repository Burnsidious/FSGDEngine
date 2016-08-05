#pragma once

#include <set>
#include <list>
#include "../EDSpatial/DynAabbTree.h"
#include "../EDGameCore/Light.h"
#include "../EDGameCore/Renderer.h"
#include "camcomp.h"

namespace EDGameCore
{
	class Camera;
	class ISkyBox;

	class Rendering
	{
	public:

		static void SetWindowWidth(int w){ windowWidth = w; }
		static void SetWindowHeight(int h){ windowHeight = h; }

		static int GetWindowWidth(void){ return windowWidth; }
		static int GetWindowHeight(void){ return windowHeight; }

		virtual void Render(void);

		static Camera* GetCurrentCamera(void){ return currentCamera; }

	protected:
		Rendering(void){}
		Rendering(const Rendering&){}
		Rendering& operator=(const Rendering&){ return *this; }
		virtual ~Rendering(){}

		friend class GameObject;

		class RenderableTreeLeaf;
		class LightTreeLeaf;

		static void Update(GameObject* obj);
		static void BeginFrame(void);

		//void RenderableAndLightTraverse( EDSpatial::DynAabbTree::Node* a, EDSpatial::DynAabbTree::Node* b, const Float3& a2bOffset, void* voidPtr );

		//static void ApplyDirectionLights(EDSpatial::DynAabbTree::Node* node);
		//static void BuildDirectionalLightVisibleSet(EDSpatial::DynAabbTree::Node* node, IDirectionalLight* light);

		//static void CameraRendererPrePass(EDSpatial::DynAabbTree::Node* node, Camera* camera, unsigned int cameraMask);
		//static void CameraLightPrePass(EDSpatial::DynAabbTree::Node* node, Camera* camera, unsigned int cameraMask);

		//static void PerRendererLeaf(RenderableTreeLeaf*, Camera* camera);
		//static void PerLightLeaf(LightTreeLeaf*, Camera* camera);

		//static void CameraPass(EDSpatial::DynAabbTree::Node* node, Camera* camera, unsigned int cameraMask, void(*PerLeaf)(RenderableTreeLeaf*, Camera*) );
		//static void CameraPass(EDSpatial::DynAabbTree::Node* node, Camera* camera, unsigned int cameraMask, void(*PerLeaf)(LightTreeLeaf*, Camera*) );

		virtual void ProcessLight(ILight* light) = 0;
		virtual void ProcessCamera(void) = 0;
		virtual void ProcessRendererPostLit(IRenderer* renderer) = 0;
		virtual void ProcessRendererPreLit(IRenderer* renderer) = 0;
		virtual void ProcessVisibleLightSet(void) = 0;
		virtual void ProcessPreLitVisibleSet(void) = 0;
		virtual void ProcessPostLitVisibleSet(void) = 0;
		virtual void PostProcessCamera() = 0;

		ISkyBox* GetSkyBox(void){ return skyBoxPtr; }

		//template<typename Func>
		//void ProcessCameraVisibleSet( Camera* camera, Func func )
		//{
		//	EDGameCore::IRenderer* renderable = camera->visibleSet;

		//	for(; renderable != 0; renderable = renderable->visibleSetNext)
		//		func( renderable );
		//}

		//template<typename Func>
		//void ProcessCameraVisibleLights( Camera* camera, Func func )
		//{
		//	EDGameCore::ILight* light = camera->visibleLightSet;

		//	for(; light != 0; light = light->visibleSetNext)
		//		func( light );
		//}

		//template<typename Func>
		//void ProcessLightVisibleSet( ILight* light, Func func )
		//{
		//	EDGameCore::IRenderer* renderable = light->visibleSetHead;

		//	for(; renderable != 0; renderable = renderable->visibleSetNext)
		//		func( renderable );
		//}

		class VisibleTreeBranch : public EDSpatial::DynAabbTree::Branch//, public VisibleTreeNode
		{
		public:
			VisibleTreeBranch(void) : cameraMask(0), frameId(0) {}
			VisibleTreeBranch* asBranch(void){ return this; }
			virtual ~VisibleTreeBranch(){}

			unsigned int cameraMask;
			unsigned int frameId;
		};

		class RenderableTreeLeaf : public EDSpatial::DynAabbTree::LeafT<IRenderer*>
		{
		public:
			RenderableTreeLeaf(void) : cameraMask(0), frameId(0) {}
			RenderableTreeLeaf* asLeaf(void){ return this; }
			virtual ~RenderableTreeLeaf(){}

			unsigned int cameraMask;
			unsigned int frameId;
		};

		class LightTreeLeaf : public EDSpatial::DynAabbTree::LeafT<IPointLight*>
		{
		public:
			LightTreeLeaf(void) : cameraMask(0), frameId(0) {}
			LightTreeLeaf* asLeaf(void){ return this; }
			virtual ~LightTreeLeaf(){}

			unsigned int cameraMask;
			unsigned int frameId;
		};

		class RenderableTree : public EDSpatial::DynAabbTreeT<IRenderer*>
		{
		private:
			RenderableTreeLeaf* createLeaf(void)const;
			VisibleTreeBranch* createBranch(void)const;
		};

		class LightTree : public EDSpatial::DynAabbTreeT<IPointLight*>
		{
		private:
			LightTreeLeaf* createLeaf(void)const;
			VisibleTreeBranch* createBranch(void)const;
		};

		void PreLitTraversal(Camera* camera, EDSpatial::DynAabbTree::Node* node);
		void PostLitTraversal(Camera* camera, EDSpatial::DynAabbTree::Node* node);
		void LightTraversal(Camera* camera, EDSpatial::DynAabbTree::Node* node );
		void LightSubTraversal(ILight* light, EDSpatial::DynAabbTree::Node* node );

		static std::set<Camera*, compstruct> cameras;
		static Camera* currentCamera;

		static std::list<IDirectionalLight*> directionalLightList;
		static IPointLight* currentLight;
		static unsigned int frameId;
		static RenderableTree renderableTree;
		static LightTree lightTree;
		static ISkyBox* skyBoxPtr;

		static int windowWidth;
		static int windowHeight;
	};
}