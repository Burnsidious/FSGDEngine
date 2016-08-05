#pragma once

#include "../tinyxml/tinyxml.h"
#include "ContentManager.h"
#include "GDAttributeTypes.h"
#include "ContentManager.h"

class GDMeshStreamBase
{
	
public:
	virtual ~GDMeshStreamBase(void){}

	virtual void SetSource(const char*) = 0;
	virtual void ReleaseSource(void) = 0;
	virtual const unsigned int* GetIndices(void) = 0;
	virtual size_t GetNumIndices(void) = 0;
	virtual const char* GetName(void) = 0;
};

class GDMeshStreamBinary
{
	EDUtilities::ContentHandle<GDAttribute<TriVertIndices>> triVertIndicesHandle;
	std::string name;
	std::string fileName;
	std::ifstream fileStream;
	unsigned int attributeOffset;
	unsigned int numAttributes;
public:

	~GDMeshStreamBinary()
	{
		ReleaseSource();
	}

	void SetSource(const char* _fileName)
	{
		fileName = _fileName;
	}

	void ReleaseSource(void)
	{
		attributeOffset = 0;
		numAttributes = 0;
		triVertIndicesHandle.Release();
		name.clear();
		fileName.clear();
		if( fileStream.is_open() )
			fileStream.close();
	}

	void OpenFile(void)
	{
		if( fileName.length() == 0 )
			return;
		if( fileStream.is_open() )
			return;

		fileStream.open( fileName.c_str(), std::ios_base::binary | std::ios_base::in );

		if( !fileStream.is_open() )
		{
			fileStream.clear();
			return;
		}

		int size = 0;
		char buffer[128];
		fileStream.read( (char*)&size, sizeof(size) ); 
		fileStream.read( buffer, size );
		buffer[size] = 0;
		name = buffer;
		
		// vertCount
		fileStream.read( (char*)&size, sizeof(size) );

		// triCount
		fileStream.read( (char*)&size, sizeof(size) );
		// seek past triIndices ( 4 bytes per index, 3 indices per triangle )
		fileStream.seekg( 4*3*size, std::ios_base::cur );
		fileStream.read( (char*)&numAttributes, sizeof(numAttributes) );
		attributeOffset = (unsigned int)fileStream.tellg();
		
		triVertIndicesHandle = EDUtilities::ContentManager::Load<GDAttribute<TriVertIndices>>( 
			fileName.c_str() );
	}

	const unsigned int* GetIndices(void)
	{	
		OpenFile();

		const unsigned int* triangleIndicesPtr = 0;
		if( triVertIndicesHandle.GetContent() != 0 )
			if( triVertIndicesHandle.GetContent()->size() != 0 )
				triangleIndicesPtr = (*triVertIndicesHandle.GetContent())[0].indices;
		
		return triangleIndicesPtr;
	}

	EDUtilities::ContentHandle<GDAttribute<TriVertIndices>> GetIndicesHandle(void)
	{	
		OpenFile();
		return triVertIndicesHandle;
	}

	size_t GetNumIndices(void)
	{
		OpenFile();
		if( triVertIndicesHandle.GetContent() != 0 )
			return triVertIndicesHandle.GetContent()->size();

		return 0;
	}

	const char* GetName(void)
	{
		OpenFile();
		return (name.length() == 0 ? 0 : name.c_str());
	}

	std::ifstream* GetAttributeTypeListPtr(const char* attributeTypeName, 
		const char* indentifier = 0)
	{
		OpenFile();

		if( !fileStream.is_open() )
			return 0;

		fileStream.clear();

		if( strcmp( attributeTypeName, "Triangle" ) == 0 )
		{
			fileStream.seekg( sizeof(unsigned int) + name.length() );
			return &fileStream;
		}

		fileStream.seekg( attributeOffset );
		
		int typeNameLen;
		char typeName[128];
		int nameLen;
		char name[128];
		int byteCount;
		int itemCount;

		while( !fileStream.eof() )
		{
			fileStream.read( (char*)&typeNameLen, sizeof(typeNameLen) );
			fileStream.read( typeName, typeNameLen );
			typeName[typeNameLen] = 0;

			fileStream.read( (char*)&nameLen, sizeof(nameLen) );
			if( nameLen != 0 )
			{
				fileStream.read( name, nameLen );
				name[nameLen] = 0;
			}

			if( strcmp( attributeTypeName, typeName ) == 0 )
			{
				bool idMatch = true;
				if( indentifier != 0 )
				{
					idMatch = false;
					if( nameLen != 0 )
						if( strcmp( indentifier, name ) == 0 )
							idMatch = true;
				}

				if( idMatch )
					return &fileStream;
			}

			if( strcmp( "BVHNode", typeName ) == 0 )
				return 0;

			fileStream.read( (char*)&byteCount, sizeof(byteCount) ); 
			fileStream.read( (char*)&itemCount, sizeof(itemCount) );
			fileStream.seekg( byteCount*itemCount, std::ios_base::cur );
		}

		return 0;
	}

	template<typename AttributeType>
	EDUtilities::ContentHandle<GDAttribute<AttributeType>> ReadAttribute(
		const char* indentifier = 0)
	{
		ContentHandle<GDAttribute<AttributeType>> handle;

		if( fileName.length() != 0 )
			handle = ContentManager::Load<GDAttribute<AttributeType>>( 
			fileName.c_str(), indentifier );

		return handle;
	}


	static GDMeshStreamBinary* Load(const char* fileName)
	{
		GDMeshStreamBinary* newMeshStreamPtr = new GDMeshStreamBinary;
		newMeshStreamPtr->SetSource(fileName);
		return newMeshStreamPtr;
	}
};

class GDMeshStreamXML : public GDMeshStreamBase
{
	TiXmlDocument* docPtr;
	TiXmlElement* attributeListElementPtr;
	EDUtilities::ContentHandle<GDAttribute<TriVertIndices>> triVertIndicesHandle;
	std::string name;
	std::string fileName;

public:
	GDMeshStreamXML(void) : docPtr(0), attributeListElementPtr(0) {}
	GDMeshStreamXML(const char* xmlFileNamePtr)
	{
		docPtr = 0;
		attributeListElementPtr = 0;

		SetSource(xmlFileNamePtr);
	}

	virtual ~GDMeshStreamXML(){ ReleaseSource(); }

	void SetSource(const char* xmlFileNamePtr)
	{
		fileName = xmlFileNamePtr;
	}

	void LoadDocument(void)
	{
		if( fileName.length() == 0 )
			return;
		if( docPtr != 0 )
			return;

		docPtr = new (TiXmlDocument)(
			(EDUtilities::ContentManager::theContentPath + fileName).c_str());
		if( !docPtr->LoadFile() )
		{
			ReleaseSource();
			return;
		}

		TiXmlElement *elementPtr = docPtr->FirstChildElement("GDMesh");

		if( elementPtr == 0 )
		{
			ReleaseSource();
			return;
		}

		name = elementPtr->Attribute("name");

		TiXmlElement* pTriListElement = elementPtr->FirstChildElement("Triangle_List");

		if(pTriListElement != 0)
			triVertIndicesHandle = 
			EDUtilities::ContentManager::LoadXML<GDAttribute<TriVertIndices>>( pTriListElement );

		attributeListElementPtr = elementPtr->FirstChildElement("GDMeshAttribute_List");
		
		if( attributeListElementPtr == 0 )
		{
			ReleaseSource();
		}
	}

	void ReleaseSource(void)
	{
		delete docPtr;
		docPtr = 0;
		attributeListElementPtr = 0;
		triVertIndicesHandle.Release();
		name.clear();
		fileName.clear();
	}

	TiXmlElement* GetAttributeTypeListElement(const char* attributeTypeName, const char* indentifier = 0)
	{
		LoadDocument();
		TiXmlElement* listElementPtr = 0;
	
		if( attributeListElementPtr != 0 )
		{
			TiXmlElement* childPtr = attributeListElementPtr->FirstChildElement("GDMeshAttribute");
	
			std::string typeList = attributeTypeName;
			typeList += "_List";

			for(; childPtr != 0; childPtr = childPtr->NextSiblingElement("GDMeshAttribute"))
			{
				TiXmlElement* pCurrent = childPtr->FirstChildElement(typeList.c_str());
				if( pCurrent != 0 )
				{
					const char* _name = pCurrent->Attribute("name");
					if( _name == 0 && indentifier == 0 )
					{
						listElementPtr = pCurrent;
						break;
					}
					else if( _name != 0 && indentifier != 0 )
					{
						if( strcmp( _name, indentifier ) == 0 )
						{
							listElementPtr = pCurrent;
							break;
						}
					}
				}
			}
		}

		return listElementPtr;
	}

	template<typename AttributeType>
	EDUtilities::ContentHandle<GDAttribute<AttributeType>> ReadAttribute(
		const char* indentifier = 0)
	{
		EDUtilities::ContentHandle<GDAttribute<AttributeType>> handle;

		if( fileName.length() != 0 )
			handle = EDUtilities::ContentManager::LoadXML<GDAttribute<AttributeType>>( 
			fileName.c_str(), indentifier );

		return handle;
	}

	const unsigned int* GetIndices(void)
	{	
		LoadDocument();
		if( triVertIndicesHandle.GetContent() != 0 )
			return (triVertIndicesHandle.GetContent()->size() == 0 ? 0 : 
			(*triVertIndicesHandle.GetContent())[0].indices);
		
		return 0;
	}

	EDUtilities::ContentHandle<GDAttribute<TriVertIndices>> GetIndicesHandle(void)
	{	
		LoadDocument();
		return triVertIndicesHandle;
	}

	size_t GetNumIndices(void)
	{
		LoadDocument();
		if( triVertIndicesHandle.GetContent() != 0 )
			return triVertIndicesHandle.GetContent()->size();

		return 0;
	}

	const char* GetName(void)
	{
		LoadDocument();
		return (name.length() == 0 ? 0 : name.c_str());
	}

	static GDMeshStreamXML* LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0)
	{
		GDMeshStreamXML* newMeshStreamPtr = new GDMeshStreamXML;
		newMeshStreamPtr->SetSource(xmlFileNamePtr);
		return newMeshStreamPtr;
	}
};