#include "../tinyxml/tinyxml.h"

namespace EDUtilities
{
    template<typename ContentType>
    ContentManager::ContentMap<ContentType>* ContentManager::ContentMap<ContentType>::instancePtr = 0;

    template<typename ContentType>
    ContentManager::ContentMap<ContentType>* ContentManager::ContentMap<ContentType>::GetInstance(void)
    {
        if( instancePtr == 0 )
        {
			instancePtr = new ContentManager::ContentMap<ContentType>;
            ContentManager::GetInstance()->AddContentMap( instancePtr );
        }

        return instancePtr;
    }

    template<typename ContentType>
    ContentManager::ContentMap<ContentType>& ContentManager::ContentMap<ContentType>::GetReference(void)
    {
        return *GetInstance();
    }

    template<typename ContentType>
    ContentManager::ContentMap<ContentType>::~ContentMap(void)
    {
        ContentManager::GetInstance()->RemoveContentMap( instancePtr );

        while( !loadedContent.empty() )
        {
			delete ((*loadedContent.begin()).second);
            loadedContent.erase( loadedContent.begin() );
        }

        instancePtr = 0;
    }

    template<typename ContentType>
    ContentHandle<ContentType> ContentManager::_load( const char* fileName )
    {
        ContentManager::ContentMap<ContentType> *contentMapPtr = ContentMap<ContentType>::GetInstance();

        std::map< std::string, ContentObject<ContentType>* >::iterator iter;

        iter = contentMapPtr->GetLoadedContent().find( std::string(fileName) );

        if( iter == contentMapPtr->GetLoadedContent().end() )
        {
			//InternalOutput::GetReference() << "Loading " << fileName << "...\n";

            ContentType* contentPtr = ContentType::Load( fileName );
            ContentObject<ContentType>* newObjectPtr = 0;

            if( contentPtr != 0 )
            {
				newObjectPtr = new ContentObject<ContentType>;
                newObjectPtr->contentPtr = contentPtr;
                newObjectPtr->fileName = fileName;
                newObjectPtr->numReferences = 0;
                contentMapPtr->GetLoadedContent().insert( std::pair< std::string, ContentObject<ContentType>* >( std::string(fileName), newObjectPtr ) );
			}
			
			if( contentMapPtr->GetLoadedContent().empty() )
				delete contentMapPtr;

            return ContentHandle<ContentType>( newObjectPtr );
        }
        else
        {
            return ContentHandle<ContentType>( (*iter).second );
        }
    }

    template<typename ContentType>
    ContentHandle<ContentType> ContentManager::_loadxml( const char* fileName, const char* indentifier )
    {
        ContentManager::ContentMap<ContentType> *contentMapPtr = ContentMap<ContentType>::GetInstance();

        std::map< std::string, ContentObject<ContentType>* >::iterator iter;

		std::string fileNameString = fileName;

		if( indentifier != 0 )
		{
			fileNameString += "::";
			fileNameString += indentifier;
		}

        iter = contentMapPtr->GetLoadedContent().find( fileNameString );

        if( iter == contentMapPtr->GetLoadedContent().end() )
        {
			//InternalOutput::GetReference() << "Loading " << fileNameString.c_str() << "...\n";
            ContentType* contentPtr = ContentType::LoadXML( fileName, indentifier );
            ContentObject<ContentType>* newObjectPtr = 0;

            if( contentPtr != 0 )
            {
				newObjectPtr = new ContentObject<ContentType>;
                newObjectPtr->contentPtr = contentPtr;
                newObjectPtr->fileName = fileNameString;
                newObjectPtr->numReferences = 0;
                contentMapPtr->GetLoadedContent().insert( std::pair< std::string, ContentObject<ContentType>* >( fileNameString, newObjectPtr ) );
			}

			if( contentMapPtr->GetLoadedContent().empty() )
				delete contentMapPtr;

            return ContentHandle<ContentType>( newObjectPtr );
        }
        else
        {
            return ContentHandle<ContentType>( (*iter).second );
        }
    }

    template<typename ContentType>
    ContentHandle<ContentType> ContentManager::_load( const char* fileName, const char* indentifier )
    {
        ContentManager::ContentMap<ContentType> *contentMapPtr = ContentMap<ContentType>::GetInstance();

        std::map< std::string, ContentObject<ContentType>* >::iterator iter;

		std::string fileNameString = fileName;

		if( indentifier != 0 )
		{
			fileNameString += "::";
			fileNameString += indentifier;
		}

		iter = contentMapPtr->GetLoadedContent().find( fileNameString );

        if( iter == contentMapPtr->GetLoadedContent().end() )
        {
            ContentType* contentPtr = ContentType::Load( fileName, indentifier );
            ContentObject<ContentType>* newObjectPtr = 0;

            if( contentPtr != 0 )
            {
				newObjectPtr = new ContentObject<ContentType>;
                newObjectPtr->contentPtr = contentPtr;
                newObjectPtr->fileName = fileNameString;
                newObjectPtr->numReferences = 0;
                contentMapPtr->GetLoadedContent().insert( std::pair< std::string, ContentObject<ContentType>* >( fileNameString, newObjectPtr ) );
			}

			if( contentMapPtr->GetLoadedContent().empty() )
				delete contentMapPtr;

            return ContentHandle<ContentType>( newObjectPtr );
        }
        else
        {
            return ContentHandle<ContentType>( (*iter).second );
        }
    }

    template<typename ContentType>
    ContentHandle<ContentType> ContentManager::_loadxml( TiXmlElement* elementPtr, const char* indentifier )
    {
        ContentManager::ContentMap<ContentType> *contentMapPtr = ContentMap<ContentType>::GetInstance();

        std::map< std::string, ContentObject<ContentType>* >::iterator iter;

		std::string elementPath = elementPtr->Value();
		TiXmlNode* parentPtr = elementPtr->Parent();
		while( parentPtr != 0 )
		{
			std::string currentPath = parentPtr->Value();
			currentPath += "::";
			currentPath += elementPath;
			elementPath = currentPath;
			parentPtr = parentPtr->Parent();
		}

		if( indentifier != 0 )
		{
			elementPath += "::";
			elementPath += indentifier;
		}

        iter = contentMapPtr->GetLoadedContent().find( elementPath );

        if( iter == contentMapPtr->GetLoadedContent().end() )
        {
			//InternalOutput::GetReference() << "Loading " << elementPath.c_str() << "...\n";

            ContentType* contentPtr = ContentType::LoadXML( elementPtr, indentifier );
            ContentObject<ContentType>* newObjectPtr = 0;

            if( contentPtr != 0 )
            {
				newObjectPtr = new ContentObject<ContentType>;
                newObjectPtr->contentPtr = contentPtr;
                newObjectPtr->fileName = elementPath;
                newObjectPtr->numReferences = 0;
                contentMapPtr->GetLoadedContent().insert( std::pair< std::string, ContentObject<ContentType>* >( elementPath, newObjectPtr ) );
			}

			if( contentMapPtr->GetLoadedContent().empty() )
				delete contentMapPtr;

            return ContentHandle<ContentType>( newObjectPtr );
        }
        else
        {
            return ContentHandle<ContentType>( (*iter).second );
        }
    }

    template<typename ContentType>
    void ContentManager::_release( ContentHandle<ContentType> &contentHandle )
    {
        ContentManager::ContentMap<ContentType> *contentMapPtr = ContentMap<ContentType>::GetInstance();

        std::map< std::string, ContentObject<ContentType>* >::iterator iter;

        iter = contentMapPtr->GetLoadedContent().find( std::string(contentHandle.GetFileName() ) );

        if( iter != contentMapPtr->GetLoadedContent().end() )
        {
            (*iter).second->numReferences -= 1;
            contentHandle.contentObjectPtr = 0;

            if( (*iter).second->numReferences == 0 )
            {
				delete (*iter).second;
                contentMapPtr->GetLoadedContent().erase( iter );

                if( contentMapPtr->GetLoadedContent().empty() )
                {
					delete contentMapPtr;
					ContentMap<ContentType>::instancePtr = 0;
				}
            }
        }
    }

    template<typename ContentType>
    ContentHandle<ContentType>::ContentHandle(const ContentHandle<ContentType>& other)
    {
        contentObjectPtr = other.contentObjectPtr;

        if( contentObjectPtr != 0 )
            contentObjectPtr->numReferences += 1;
    }

    template<typename ContentType>
    ContentHandle<ContentType>& ContentHandle<ContentType>::operator=(const ContentHandle<ContentType>& other)
    {
        if( &other != this )
        {
            if( contentObjectPtr != 0 )
                ContentManager::GetInstance()->Release(*this);

            contentObjectPtr = other.contentObjectPtr;

			if( contentObjectPtr != 0 )
	            contentObjectPtr->numReferences += 1;
        }

        return *this;
    }

    template<typename ContentType>
    ContentHandle<ContentType>& ContentHandle<ContentType>::operator=( ContentHandle<ContentType>&& other)
    {
        if( this !=  &other)
        {
            if( contentObjectPtr != 0 )
                ContentManager::GetInstance()->Release(*this);

			contentObjectPtr = std::move(other.contentObjectPtr);

			if( contentObjectPtr != 0 )
	            contentObjectPtr->numReferences += 1;
        }

        return *this;
    }

    template<typename ContentType>
    ContentHandle<ContentType>::ContentHandle(ContentObject<ContentType>* _contentObjectPtr)
    {
        contentObjectPtr = _contentObjectPtr;
		
		if( contentObjectPtr != 0 )
			contentObjectPtr->numReferences += 1;
    }

    template<typename ContentType>
    ContentHandle<ContentType>::~ContentHandle(void)
    {
		Release();
    }

	template<typename ContentType>
	void ContentHandle<ContentType>::Release(void)
	{
		if( contentObjectPtr != 0 )
		{
			ContentManager::GetInstance()->Release(*this);
			contentObjectPtr = 0;
		}
	}

	template<typename ContentType>
	void ContentHandle<ContentType>::ReleaseIfLast(void)
	{
		if( contentObjectPtr != 0 )
		{
			if( contentObjectPtr->numReferences == 1 )
			{
				ContentManager::GetInstance()->Release(*this);
				contentObjectPtr = 0;
			}
		}
	}
}