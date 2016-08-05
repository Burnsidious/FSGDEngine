#include "GDMesh.h"

#include "../tinyxml/tinyxml.h"
#include "../EDUtilities/ContentManager.h"

using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::string;

namespace EDUtilities
{
    GDMesh::GDMesh(void)
    {
        numVerts = 0;
    }

    GDMesh::GDMesh(const char* filename)
    {
        numVerts = 0;

        ifstream fin(filename, ios_base::in | ios_base::binary);

        if( fin.is_open() )
        {
            Read(fin);
			
			fin.close();
        }
    }

    void GDMesh::Unload(void)
    {
        name.clear();
        indices.clear();
        numVerts = 0;

        attributes.clear();
    }

    void GDMesh::SetName(const char* _name)
    {
        name = _name;
    }

    void GDMesh::SetIndices(const int* _indices, int _numIndices)
    {
        indices.resize(_numIndices);
        memcpy_s( &indices[0], sizeof(int)*_numIndices, _indices, sizeof(int)*_numIndices );
    }

    void GDMesh::SetNumVerts(int _numVerts) 
    { 
        numVerts = _numVerts; 
    }

    void GDMesh::SetAttributes(const GDMeshAttribute* _attributes, int numAttributeSets)
    {
        attributes.clear();
        attributes.reserve( numAttributeSets );
        while( numAttributeSets != 0 )
            attributes.push_back( _attributes[--numAttributeSets] );
    }

    const GDMeshAttribute* GDMesh::GetAttribute(const char* attributeName) const
    {
		size_t vSize = attributes.size();
        for(unsigned int i = 0; i < vSize; ++i)
            if( _stricmp( attributes[i].GetAttributeName(), attributeName ) == 0 )
                return &attributes[i];

        return 0;
    }

    void GDMesh::SetAttribute(const char* attributeName, const char* attributeData, int numBytes)
    {
		size_t vSize = attributes.size();
		for(unsigned int i = 0; i < vSize; ++i)
		{
			if( _stricmp( attributes[i].GetAttributeName(), attributeName ) == 0 )
			{
				attributes[i].SetAttribute( attributeName, attributeData, numBytes );
				return;
			}
		}
		attributes.push_back( GDMeshAttribute() );
		attributes[ attributes.size()-1 ].SetAttribute( attributeName, attributeData, numBytes );
	}

    void GDMesh::RemoveAttribute(const char* attributeName)
    {
        std::vector<GDMeshAttribute>::iterator iter = attributes.begin();

        for(; iter != attributes.end(); ++iter)
            if( _stricmp( (*iter).GetAttributeName(), attributeName ) == 0 )
            {
                attributes.erase( iter );
                return;
            }
    }

    const GDMeshAttribute* GDMesh::GetAttributes(void) const 
    { 
        return (attributes.empty() ? 0 : &(attributes[0]));
    }

    void GDMesh::Read(ifstream &fin)
    {
        Unload();

        int nameLen = 0;
        fin.read( (char*)&nameLen, sizeof(nameLen) );
		char *buffer = new char[nameLen+1];
        buffer[nameLen] = 0;
        fin.read( buffer, sizeof(char)*nameLen );
        name = buffer;
		delete [] buffer;

        int numIndices = 0;
        fin.read( (char*)&numIndices, sizeof(numIndices) );
        fin.read( (char*)&numVerts, sizeof(numVerts) );

        if(	numIndices != 0 )
        {
            indices.resize(numIndices);
            fin.read( (char*)&indices[0], sizeof(int)*numIndices );
        }

        int numAttributeSets = 0;
        fin.read( (char*)&numAttributeSets, sizeof(numAttributeSets) );

        attributes.resize(numAttributeSets);
        while( numAttributeSets != 0 )
            attributes[ --numAttributeSets ].Read( fin );
    }

    void GDMesh::Write(ofstream &fout)
    {
        unsigned int nameLen = (unsigned int)name.length();
        fout.write( (char*)&nameLen, sizeof(nameLen) );
        fout.write( name.c_str(), sizeof(char)*nameLen );

        unsigned int numIndices = (unsigned int)indices.size();
        fout.write(	(char*)&numIndices, sizeof(numIndices) );
        fout.write( (char*)&numVerts, sizeof(numVerts) );

        if(	numIndices != 0 )
            fout.write( (char*)&indices[0], sizeof(int)*numIndices );

        unsigned int numAttributeSets = (unsigned int)attributes.size();
        fout.write( (char*)&numAttributeSets, sizeof(numAttributeSets) );

        while( numAttributeSets != 0 )
            attributes[--numAttributeSets].Write( fout );
    }

    GDMesh* GDMesh::Load(const char* filename)
    {
        GDMesh* meshPtr = 0;

        ifstream fin(filename, ios_base::in | ios_base::binary);

        if( fin.is_open() )
        {
			meshPtr = new GDMesh;
            meshPtr->Read(fin);

			//meshPtr->SaveXML();

            fin.close();
        }
	
        return meshPtr;
    }

    void GDMesh::Save(const char* filename, bool overwrite)
    {
        ifstream fin( filename );
        fin.close();

        if( fin.fail() || overwrite )
        {
            fin.clear(ios_base::failbit);

            ofstream fout( filename, ios_base::out | ios_base::binary | ios_base::trunc );

            if( !fout.bad() )
            {
                Write(fout);
                fout.close();
            }
        }
    }

	void GDMesh::SaveXML(void) const
	{
		std::string fileName = ContentManager::theContentPath + "/gdm_";
		fileName += name;
		fileName += ".xml";

		std::ofstream fout( fileName.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
		
		fout << "<?xml version=\"1.0\"?>\n";
		
		WriteXML( fout );

		fout.close();
	}

	void GDMesh::WriteXML(std::ofstream &fout) const
	{
		char buffer[128];
		fout << "<GDMesh name=\"" << name.c_str() << "\">\n";
		sprintf_s(buffer, "%d", numVerts);
		fout << "\t<NumVerts>" << buffer << "</NumVerts>\n";

		size_t numTriangles = indices.size()/3;

		sprintf_s(buffer, "\t<Triangle_List count=\"%d\">", numTriangles );
		fout << buffer;

		for(unsigned int i = 0; i < numTriangles; ++i)
		{
			sprintf_s(buffer, "\t\t<Triangle n=\"%d\">\n", i );
			fout << buffer;
			sprintf_s(buffer, "\t\t\t<Index n=\"%d\">", 0 );
			fout << buffer;
			sprintf_s(buffer, "%d</Index>\n", indices[i*3]);
			fout << buffer;
			sprintf_s(buffer, "\t\t\t<Index n=\"%d\">", 1 );
			fout << buffer;
			sprintf_s(buffer, "%d</Index>\n", indices[i*3+1]);
			fout << buffer;
			sprintf_s(buffer, "\t\t\t<Index n=\"%d\">", 2 );
			fout << buffer;
			sprintf_s(buffer, "%d</Index>\n", indices[i*3+2]);
			fout << buffer;
			fout << "\t\t</Triangle>\n";
		}

		fout << "\t</Triangle_List>\n";

		sprintf_s(buffer, "\t<GDMeshAttribute_List count=\"%d\">\n", attributes.size());
		fout << buffer;
		size_t vSize = attributes.size();
		for(unsigned int i = 0; i < vSize; ++i )
			attributes[i].WriteXML(fout);

		fout << "\t</GDMeshAttribute_List>\n";

		fout << "</GDMesh>\n";
	}

	void GDMesh::OpenXML(const char* filename)
	{
		Unload();

		TiXmlDocument doc(filename);

		if(!doc.LoadFile())
			return;

		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;
		TiXmlHandle hRoot(0);
		
		elementPtr = hDoc.FirstChildElement().Element();
		if( !elementPtr )
			return;

		std::string rootName = elementPtr->Value();

		if( strcmp(rootName.c_str(), "GDMesh") != 0 )
			return;

		hRoot = TiXmlHandle(elementPtr);

		name = elementPtr->Attribute("name");

		elementPtr = hRoot.FirstChild( "NumVerts" ).Element();

		if( elementPtr != 0 )
			numVerts = atoi(elementPtr->GetText());

		elementPtr = hRoot.FirstChild( "Triangle_List" ).Element();

		if( elementPtr != 0 )
		{
			unsigned int numIndices = atoi(elementPtr->Attribute("count") ) * 3;
			indices.resize( numIndices );

			elementPtr = elementPtr->FirstChildElement("Triangle");

			while( elementPtr != NULL )
			{
				unsigned int triIndex = atoi(elementPtr->Attribute("n"));

				TiXmlElement *childElementPtr = elementPtr->FirstChildElement("Index");
				while( childElementPtr != NULL )
				{
					unsigned int componentIndex = atoi( childElementPtr->Attribute("n") );
					unsigned int index = triIndex*3 + componentIndex;
					
					if( index < numIndices )
						indices[index] = atoi(childElementPtr->GetText());

					childElementPtr = childElementPtr->NextSiblingElement("Index");
				}
				
				elementPtr = elementPtr->NextSiblingElement("Triangle");
			}
			/*
			TiXmlElement* pChildElement = elementPtr->FirstChildElement("Index");
			while( pChildElement != NULL )
			{
				unsigned int index = atoi(pChildElement->Attribute("n"));
				if( index >= indices.size() )
					continue;

				indices[index] = atoi( pChildElement->GetText() );

				pChildElement = pChildElement->NextSiblingElement("Index");
			}
			*/
		}

		elementPtr = hRoot.FirstChild("GDMeshAttribute_List").Element();

		if( elementPtr != 0 )
		{
			int count = atoi( elementPtr->Attribute("count") );

			attributes.resize( count );

			elementPtr = elementPtr->FirstChildElement("GDMeshAttribute");

			int index = 0;
			while( elementPtr != NULL )
			{
				attributes[index].ReadXML(elementPtr);
				
				elementPtr = elementPtr->NextSiblingElement("GDMeshAttribute");
				if( ++index == count )
					break;
			}
		}
	}
}