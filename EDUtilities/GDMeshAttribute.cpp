
#include "GDMeshAttribute.h"

#include "../EDUtilities/ContentManager.h"

using std::ifstream;
using std::ofstream;
using std::vector;
using std::ios_base;

namespace EDUtilities
{
    const char* GDMeshAttribute::GetAttributeName(void) const
    {
        return attributeName.c_str();
    }

    const char* GDMeshAttribute::GetAttributeData(void) const
    {
        return (attributeData.empty() ? 0 : &attributeData[0]);
    }

    void GDMeshAttribute::SetAttribute( const char* _attributeName, const char* _attributeData, int numBytes)
    {
        attributeName = _attributeName;
        attributeData.resize(numBytes);
        memcpy_s( &attributeData[0], sizeof(char)*numBytes, _attributeData, sizeof(char)*numBytes );
    }

    void GDMeshAttribute::Read(ifstream &fin)
    {
        char buffer[GDMESH_ATTRIB_NAMELEN+1];
        fin.read( buffer, sizeof(char)*GDMESH_ATTRIB_NAMELEN );
        buffer[GDMESH_ATTRIB_NAMELEN] = 0;
        attributeName = buffer;

        int numBytes = 0;
        fin.read( (char*)&numBytes, sizeof(numBytes) );

        attributeData.resize( numBytes );
        fin.read( &attributeData[0], sizeof(char)*numBytes );

		return;
    }

    void GDMeshAttribute::Write(ofstream &fout)
    {
        char buffer[GDMESH_ATTRIB_NAMELEN+1];
        memset( buffer, 0, sizeof(buffer) );
        strcpy_s( buffer, GDMESH_ATTRIB_NAMELEN+1, attributeName.c_str() );

        fout.write( buffer, sizeof(char)*GDMESH_ATTRIB_NAMELEN );

        unsigned int numBytes = (unsigned int)attributeData.size();
        fout.write( (char*)&numBytes, sizeof(numBytes) );
        if( numBytes != 0 )
            fout.write( &attributeData[0], sizeof(char)*numBytes );
    }

    size_t GDMeshAttribute::GetAttributeByteSize(void)
    {
        return attributeData.size();
    }

	void GDMeshAttribute::SaveXML(void) const
	{
		std::string fileName = ContentManager::theContentPath + "/gdma_";
		fileName += attributeName;
		fileName += ".xml";

		std::ofstream fout( fileName.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
		
		fout << "<?xml version=\"1.0\"?>\n";
		
		WriteXML( fout );

		fout.close();
	}

	void GDMeshAttribute::ReadXML(TiXmlElement *elementPtr)
	{
		if( !elementPtr )
			return;

		std::string rootName = elementPtr->Value();

		if( strcmp(rootName.c_str(), "GDMeshAttribute") != 0 )
			return;

		attributeName = elementPtr->Attribute("name");

		std::string::size_type formatStart = attributeName.find_last_of('_');

		std::string format = attributeName.substr( formatStart+1 );

		TiXmlElement *childPtr = NULL;

		int typeSize = -1;
		int type = -1;

		char typeChar = format.c_str()[ format.length() - 1];

		if( typeChar == 'f' )
		{
			typeSize = 4;
			type = 0;
		}
		else if( typeChar == 'd' )
		{
			typeSize = 8;
			type = 1;
		}
		else if( typeChar == 'i' )
		{
			typeSize = 4;
			type = 2;
		}
		else if( typeChar == 'c' )
		{
			typeSize = 1;
			type = 3;
		}
		else if( typeChar == 's' )
		{
			typeSize = 2;
			type = 4;
		}
		else if( typeChar == 'b' )
		{
			typeSize = 1;
			type = 5;
		}
		else if( typeChar == 't' )
		{
			typeSize = 0;
			type = 6;
		}

		format = format.substr(0, format.length()-1);
		int componentCount = atoi(format.c_str());

		std::string _attributeName = attributeName.substr( 0, formatStart );
		
		childPtr = elementPtr->FirstChildElement( (_attributeName + "_List").c_str() );
		int attributeCount = atoi(childPtr->Attribute("count"));

		attributeData.resize( componentCount*attributeCount*typeSize );
		memset( &attributeData[0], 0,attributeData.size() );
		
		childPtr = childPtr->FirstChildElement( _attributeName.c_str() );
		while( childPtr != 0 )
		{
			int index = atoi(childPtr->Attribute("n"));
			int byteIndex = index * componentCount * typeSize;

			TiXmlElement* pComponent = childPtr->FirstChildElement("Component");
			
			for( int i = 0; i < componentCount; ++i )
			{
				if( pComponent == NULL )
					break;

				int componentIndex = atoi(pComponent->Attribute("n"));

				if( componentIndex >= componentCount )
					continue;

				int componentByteIndex = byteIndex + (componentIndex*typeSize);

				if( typeChar == 'f' )
				{
					float value = (float)atof( pComponent->GetText() );	
					memcpy_s( &attributeData[componentByteIndex], typeSize, &value, typeSize );
				}
				else if( typeChar == 'd' )
				{
					double value = atof( pComponent->GetText() );	
					memcpy_s( &attributeData[componentByteIndex], typeSize, &value, typeSize );
				}
				else if( typeChar == 'i' )
				{
					int value = atoi( pComponent->GetText() );	
					memcpy_s( &attributeData[componentByteIndex], typeSize, &value, typeSize );
				}
				else if( typeChar == 'c' )
				{
					attributeData[componentByteIndex] = pComponent->GetText()[0];
				}
				else if( typeChar == 's' )
				{
					short value = (short)atoi( pComponent->GetText() );	
					memcpy_s( &attributeData[componentByteIndex], typeSize, &value, typeSize );					
				}
				else if( typeChar == 'b' )
				{
					attributeData[componentByteIndex] = pComponent->GetText()[0];
				}
				else if( typeChar == 't' )
				{
					
				}

				pComponent = pComponent->NextSiblingElement("Component");
			}

			childPtr = childPtr->NextSiblingElement( _attributeName.c_str() );
		}
	}

	void GDMeshAttribute::OpenXML(const char* fileName)
	{
		TiXmlDocument doc(fileName);

		if(!doc.LoadFile())
			return;

		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;
		TiXmlHandle hRoot(0);
		
		elementPtr = hDoc.FirstChildElement().Element();

		ReadXML(elementPtr);
	}

	void GDMeshAttribute::WriteXML(std::ofstream &fout) const
	{
		fout << "\t\t<GDMeshAttribute name=\"" << attributeName.c_str() << "\">\n";

		std::string::size_type formatStart = attributeName.find_last_of("_"); 

		if( formatStart != std::string::npos )
		{
			char type = tolower(attributeName.c_str()[attributeName.length()-1]);
			int count = atoi( attributeName.substr( formatStart+1, attributeName.length() - (formatStart+2) ).c_str() );

			const char types[] = { 'f', 'd', 'i', 'c', 's', 'b', 't' };
			const char *typeStrings[] = { "float", "double", "int", "byte", "short", "bool", "string" };
			const char *formatString[] = { "%f", "%f", "%d", "%c", "%d", "%d", "\"%s\""};
			int sizes[] = { 4, 8, 4, 1, 2, 1, 0 };
			
			int typeIndex = 0;

			for(; typeIndex < sizeof(types); ++typeIndex)
				if( type == types[typeIndex] )
					break;

			if( typeIndex != sizeof(types) )
			{				
				size_t num = 0;
				size_t byteIndex = 0;

				if( type == 't' )
				{
					size_t vSize = attributeData.size();
					while( byteIndex < vSize)
					{
						++num;
						byteIndex += strlen( &(attributeData[byteIndex]) ) + 1;
					}

					byteIndex = 0;
				}
				else
					num = attributeData.size() / (count * sizes[typeIndex]);

				char buffer[128];

				std::string _attributeName = attributeName.substr( 0, formatStart );

				sprintf_s( buffer, "\t\t\t<%s_List count=\"%d\">\n", _attributeName.c_str(), num );
				fout << buffer;

				for( size_t i = 0; i < num; ++i )
				{
					sprintf_s( buffer, " n=\"%d\">", i );
					fout <<  "\t\t\t\t<" << _attributeName.c_str() << buffer;

					for(int j = 0; j < count; ++j)
					{
						sprintf_s( buffer, "\t\t\t\t\t<Component n=\"%d\">", j );
						fout << buffer;
						switch( type )
						{
						case 'f':
							{
								sprintf_s(buffer, formatString[typeIndex], *((float*)(&attributeData[ sizes[typeIndex]*count*i + sizes[typeIndex]*j ])));
							}
							break;
						case 'd':
							{
								sprintf_s(buffer, formatString[typeIndex], *((double*)(&attributeData[ sizes[typeIndex]*count*i + sizes[typeIndex]*j ])));
							}
							break;
						case 'i':
							{
								sprintf_s(buffer, formatString[typeIndex], *((int*)(&attributeData[ sizes[typeIndex]*count*i + sizes[typeIndex]*j ])));
							}
							break;
						case 'c':
							{
								sprintf_s(buffer, formatString[typeIndex], *((char*)(&attributeData[ sizes[typeIndex]*count*i + sizes[typeIndex]*j ])));
							}
							break;
						case 's':
							{
								sprintf_s(buffer, formatString[typeIndex], *((short*)(&attributeData[ sizes[typeIndex]*count*i + sizes[typeIndex]*j ])));
							}
							break;
						case 'b':
							{
								sprintf_s(buffer, formatString[typeIndex], *((char*)(&attributeData[ sizes[typeIndex]*count*i + sizes[typeIndex]*j ])));
							}
							break;
						case 't':
							{
							}
							break;
						default:
							break;
						}

						fout << buffer;
						fout << "</Component>\n";
					}

					fout << "\t\t\t\t</" << _attributeName.c_str() << ">\n";
				}
				
				fout << "\t\t\t</" << _attributeName.c_str() << "_List>\n";
			}
		}

		fout << "\t\t</GDMeshAttribute>\n";
	}
}