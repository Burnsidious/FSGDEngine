#pragma once

#include "../EDUtilities/InternalOutput.h"

class TiXmlElement;

namespace EDUtilities
{
	template<typename ContentType>
	class ContentObject
	{
		friend class ContentManager;

		template<typename T>
		friend class ContentHandle;

	private:
		std::string fileName;
		ContentType* contentPtr;
		unsigned int numReferences;

	public:

		ContentObject(void) : contentPtr(0), numReferences(0) {}
		~ContentObject(void)
		{ 
			delete contentPtr; 
		}
		const std::string& GetFileName(void) const { return fileName; }

		ContentType* GetContent(void) const { return contentPtr; }

		unsigned int GetRefCount(void) const { return numReferences; }
	};

	template<typename ContentType>
	class ContentHandle
	{
		friend class ContentManager;

	private:
		ContentObject<ContentType> *contentObjectPtr;
		ContentHandle(ContentObject<ContentType>* _contentObjectPtr);

	public:
		ContentHandle(void) : contentObjectPtr(0) {}
		~ContentHandle(void);
		ContentHandle(const ContentHandle<ContentType>& other);
		ContentHandle& operator=(const ContentHandle<ContentType>& other);
		ContentHandle& operator=(ContentHandle<ContentType>&& other);

		ContentType* GetContent(void) const { return (contentObjectPtr == 0 ? 0 : contentObjectPtr->GetContent()); }
		const char* GetFileName(void) const { return (contentObjectPtr == 0 ? 0 : contentObjectPtr->GetFileName().c_str()); }
		void Release(void);
		void ReleaseIfLast(void);
	};

	class ContentManager
	{
	private:

		class ContentMapInterface
		{
		public:
			virtual ~ContentMapInterface(void) = 0 {}
		};

		template<typename ContentType>
		class ContentMap : public ContentMapInterface
		{
			friend class ContentManager;
		private:
			ContentMap(void){}
			ContentMap(const ContentMap&){}
			ContentMap& operator=(const ContentMap&){}


			static ContentMap* GetInstance(void);
			static ContentMap& GetReference(void);

			static ContentMap* instancePtr;

			std::map< std::string, ContentObject<ContentType>* > loadedContent;
		public:
			// CHANGED TO PUBLIC Destructor for Memory Manager Integration!!!
			// TODO
			// MMTODO
			~ContentMap(void);
			std::map< std::string, ContentObject<ContentType>* >& GetLoadedContent(void){ return loadedContent; }

		};

		ContentManager(void){};
		ContentManager(const ContentManager&){}
		ContentManager& operator=(const ContentManager&){ return *this; }


		static ContentManager* instancePtr;
		std::set< ContentMapInterface* > contentMaps;

		template<typename ContentType>
		ContentHandle<ContentType> _load( const char* fileName );

		template<typename ContentType>
		ContentHandle<ContentType> _load( const char* fileName, const char* indentifier );

		template<typename ContentType>
		ContentHandle<ContentType> _loadxml( const char* fileName, const char* indentifier = 0 );

		template<typename ContentType>
		ContentHandle<ContentType> _loadxml( TiXmlElement* elementPtr, const char* indentifier = 0 );

		template<typename ContentType>
		void _release( ContentHandle<ContentType> &contentHandle );

	public:
		static std::string theContentPath;

		// CHANGED TO PUBLIC Destructor for Memory Manager Integration!!!
		// TODO
		// MMTODO
		~ContentManager(void);
		static ContentManager* GetInstance(void);
		static ContentManager& GetReference(void);
		static void DeleteInstance(void);

		bool AddContentMap( ContentMapInterface* contentMapPtr );

		bool RemoveContentMap( ContentMapInterface* contentMapPtr );

		template<typename ContentType>
		static ContentHandle<ContentType> Load( const char* fileName )
		{
			return GetInstance()->_load<ContentType>( fileName );
		}

		template<typename ContentType>
		static ContentHandle<ContentType> Load( const char* fileName, const char* indentifier )
		{
			return GetInstance()->_load<ContentType>( fileName, indentifier );
		}

		template<typename ContentType>
		static ContentHandle<ContentType> LoadXML( const char* fileName, const char* indentifier = 0 )
		{
			return GetInstance()->_loadxml<ContentType>( fileName, indentifier );
		}

		template<typename ContentType>
		static ContentHandle<ContentType> LoadXML( TiXmlElement* elementPtr, const char* indentifier = 0 )
		{
			return GetInstance()->_loadxml<ContentType>( elementPtr, indentifier );
		}

		template<typename ContentType>
		static void Release( ContentHandle<ContentType> &contentHandle )
		{
			GetInstance()->_release( contentHandle );
		}
	};
}
#include "ContentManager.hpp"

