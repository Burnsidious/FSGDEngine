
#include "ContentManager.h"
using std::string;

namespace EDUtilities
{
	string ContentManager::theContentPath;

    ContentManager* ContentManager::instancePtr = 0;

    ContentManager::~ContentManager(void)
    {
        while( !contentMaps.empty() )
		{
           delete (*contentMaps.rbegin());
		}
    }

    ContentManager* ContentManager::GetInstance(void)
    {
        if( 0 == instancePtr )
            instancePtr = new ContentManager;

        return instancePtr;
    }

    ContentManager& ContentManager::GetReference(void)
    {
        return *GetInstance();
    }

    void ContentManager::DeleteInstance(void)
    {
        //instancePtr->contentMaps.clear();

        delete instancePtr;
		instancePtr = 0;
    }

    bool ContentManager::AddContentMap( ContentMapInterface* contentMapPtr )
    {
        return contentMaps.insert( contentMapPtr ).second;
    }

    bool ContentManager::RemoveContentMap( ContentMapInterface* contentMapPtr )
    {
        std::set<ContentMapInterface*>::iterator iter = contentMaps.find( contentMapPtr );

        if( iter != contentMaps.end() )
        {
            contentMaps.erase( iter );
            return true;
        }

        return false;
    }

}
