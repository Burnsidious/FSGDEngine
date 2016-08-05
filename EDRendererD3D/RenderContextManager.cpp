#include "RenderContextManager.h"

namespace EDRendererD3D
{
    RenderContextManager *RenderContextManager::m_pInstance = 0;

    RenderContextManager::RenderContextManager()
    {
    }

    RenderContextManager::~RenderContextManager()
    {
    }

    RenderContextManager *RenderContextManager::GetInstance()
    {
        if(0 == m_pInstance)
        {
            m_pInstance = MMNew(RenderContextManager);
        }
        return m_pInstance;
    }

    RenderContextManager &RenderContextManager::GetReference()
    {
        return *GetInstance();
    }

    void RenderContextManager::DeleteInstance()
    {
        MMDel(m_pInstance);
        m_pInstance = 0;
    }

    RenderContext *RenderContextManager::GetContextByName(const char* pContextName)
    {
        //MMTODO
        std::string contextName = pContextName;
        //MMTODO
        std::map< std::string, RenderContext* >::iterator iter = m_ContextMap.find( contextName );

        if( iter == m_ContextMap.end() )
            return 0;
        else
            return (*iter).second;
    }
}