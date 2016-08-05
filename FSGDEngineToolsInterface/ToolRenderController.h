#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <list>

#include "../EDRendererD3D/RenderContext.h"

namespace FSGDEngineToolInterface
{
	class ToolRenderController
	{

	public:
		~ToolRenderController(void);
		static ToolRenderController *GetInstance(void);
		static ToolRenderController &GetReference(void);

		static void DeleteInstance(void);

		void Initialize(HWND hWnd);
		void Render(void);
		void Shutdown(void);

	private:
		ToolRenderController(void);

		static ToolRenderController *instancePtr;

		// Hide access for singleton model
		ToolRenderController(const ToolRenderController &) {}
		ToolRenderController(const ToolRenderController &&) {}
		ToolRenderController &operator=(const ToolRenderController &) { return *this;}
		ToolRenderController &operator=(const ToolRenderController &&) { return *this;}


		////////////////////////////////////////////////////////////////////////////////
		// Default contexts that should always exist
		/// RenderContext to be used for rendering the 2D objects.
		/// This context is added to the RenderContext Manager with the name 2DContext.
		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> twoDContext;
		/// A render context used to render directional lights in a deferred renderer.
		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> directionDeferredContextHandle;
		/// A render context used to render point lights in a deferred renderer.
		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> pointLightContextHandle;
		/// A render context used to render spot lights in a deferred renderer.
		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> spotLightContextHandle;
		/// Render context used for debug line rendering
		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> debugLines;
		/// Render context used for debug triangle rendering
		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> debugTriangles;
		/// Render Context used for rendering standard transparent objects
		EDRendererD3D::RenderContext* transparentContextPtr;

		////////////////////////////////////////////////////////////////////////////////
		// Data driven contexts
		/// A collection of contexts that hold data that should cast shadows.
		std::list<EDRendererD3D::RenderContext *> shadowCasters;
		/// A collection of contexts that hold data to be rendered to basic G-Buffer
		std::list<EDRendererD3D::RenderContext *> gBufferContexts;
		/// A collection of contexts that hold data to be rendered to depth target
		std::list<EDRendererD3D::RenderContext *> depthCameraContexts;
		/// A collection of contexts that hold data to be rendered without lighting 
		/// during the forward rendering pass
		std::list<EDRendererD3D::RenderContext *> unlitOpaqueContexts;

		////////////////////////////////////////////////////////////////////////////////
		// Internal use methods
		void CreateContexts(void);

		/// Method used to add RenderContexts to be rendered, puts the contexts in the appropriate
		/// list based on state.
		void AddRenderContext(EDRendererD3D::RenderContext* pContext);
	};
}
