#pragma once
#include "RenderContext.h"
#include <map>

namespace EDRendererD3D
{
	/// A singleton class used to manage RenderContext instances.
    class RenderContextManager
    {
    private:
        /// Private constructor used to ensure singleton state.
        RenderContextManager();
        /// Private copy constructor used to ensure singleton state.
        RenderContextManager(const RenderContextManager &) {}
        /// Private assignment operator used to ensure singleton state.
        RenderContextManager &operator=(const RenderContextManager &) {}

		/// The one and only instance of this class.
        static RenderContextManager *m_pInstance;

		/// The container of RenderContext pointers.  The name of the context is used 
		/// to define the map key and must be unique.
    	std::map< std::string, RenderContext* > m_ContextMap;

    public:
        /// Gets a pointer to the instance.
        static RenderContextManager *GetInstance();
        /// Gets a reference to the instance.
        static RenderContextManager &GetReference();
        /// Cleans up the instance.  Must be called to prevent memory leaks. 
        static void DeleteInstance();

        ~RenderContextManager();

		/// Adds a RenderContext to be managed.
		/// \param context - The address of a RenderContext to be managed.  The context does not need to be unique.
		/// \param name - A unique name to be used for correlation with the context being added.
        void AddRenderContext(RenderContext *context, const char *name)
        {
            m_ContextMap.insert( std::pair< std::string, RenderContext* >(std::string(name), context));
        }
		/// Gets a RenderContext being managed.
		/// \param pContextName - The name of the context to retrieve.
		/// \return The context with the given name or null if no context exists with that name.
    	RenderContext *GetContextByName(const char* pContextName);
    };
}
