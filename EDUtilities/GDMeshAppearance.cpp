
#include "GDMeshAppearance.h"
#include "../tinyxml/tinyxml.h"
#include "../EDUtilities/ContentManager.h"

using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::vector;
using std::string;

namespace EDUtilities
{
	const char* GDMeshAppearance::theAppearanceFlagStrings[] = { "Indexed", "Textured", "Lit", "Alpha", "CubeMap" };

    GDMeshAppearance::GDMeshAppearance(void) : flags(0) {}

    GDMeshAppearance::GDMeshAppearance(const char* fileName) : flags(0)
    {
        ifstream fin(fileName, ios_base::in | ios_base::binary);

        if( fin.is_open() )
        {
            Read( fin );
            fin.close();
        }
    }

    GDMeshAppearance::GDMeshAppearance(ifstream &fin)
    {
        flags = 0;
        Read( fin );
    }

    const char* GDMeshAppearance::GetName(void) const
    { 
        return name.c_str();
    }

    void GDMeshAppearance::SetName(const char* _name)
    {
        name = _name;
    }

    unsigned int GDMeshAppearance::GetAppearanceFlags(void) const
    { 
        return flags;
    }

    void GDMeshAppearance::SetAppearanceFlags(unsigned int _flags)
    { 
        flags = _flags;
    }

    const char* GDMeshAppearance::GetContextName(void) const
    {
        return contextName.c_str();
    }

    void GDMeshAppearance::SetContextName(const char* _contextName)
    {
        contextName = _contextName;
    }

    const vector< string >& GDMeshAppearance::GetTextureFileNames(void) const
    { 
        return textureNames;
    }

    void GDMeshAppearance::SetTextureFileNames(vector< string >& names)
    { 
        textureNames = names;
    }

    void GDMeshAppearance::AddTextureFileName(const char* _textureName)
    { 
        textureNames.push_back(string(_textureName) );
    }

    void GDMeshAppearance::RemoveTextureFileName(const char* _textureName)
    {
        vector< string >::iterator iter = textureNames.begin();

        for(; iter != textureNames.end(); ++iter)
        {
            if( _stricmp((*iter).c_str(), _textureName) == 0 )
            {
                textureNames.erase( iter );
                break;
            }
        }
    }

    const char* GDMeshAppearance::GetMeshFileName(void) const
    { 
        return meshFileName.c_str();
    }

    void GDMeshAppearance::SetMeshFileName(const char* _meshFileName)
    { 
        meshFileName = _meshFileName;
    }

    void GDMeshAppearance::Read(ifstream &fin)
    {
        int nameLen;
        fin.read( (char*)&nameLen, sizeof(nameLen) );
		char* _name = new char[nameLen+1];
        _name[nameLen] = 0;
        fin.read( _name, nameLen );
        name = _name;
		delete []_name;

        fin.read( (char*)&flags, sizeof(flags) );

        fin.read( (char*)&nameLen, sizeof(nameLen) );
		_name = new char[nameLen+1];
        _name[nameLen] = 0;
        fin.read( _name, nameLen );
        contextName = _name;
		delete []_name;

        int texCount;
        fin.read( (char*)&texCount, sizeof(texCount) );

        textureNames.resize(texCount);
        for(int i = 0; i < texCount; ++i)
        {
            fin.read( (char*)&nameLen, sizeof(nameLen) );
			_name = new char[nameLen+1];
            _name[nameLen] = 0;
            fin.read( _name, nameLen );
            textureNames[i] = _name;
			delete []_name;
        }

        fin.read( (char*)&nameLen, sizeof(nameLen) );
		_name = new char[nameLen+1];
        _name[nameLen] = 0;
        fin.read( _name, nameLen );
        meshFileName = _name;
		delete []_name;
    }

    void GDMeshAppearance::Write(ofstream &fout) const
    {
        unsigned int count = (unsigned int)name.length();
        fout.write( (char*)&count, sizeof(count) );
        fout.write( name.c_str(), count );

        fout.write( (char*)&flags, sizeof(flags) );

        count = (unsigned int)contextName.length();
        fout.write( (char*)&count, sizeof(count) );
        fout.write( contextName.c_str(), count );

        count = (unsigned int)textureNames.size();
        fout.write( (char*)&count, sizeof(count) );

        for(size_t i = 0; i < count; ++i)
        {
            unsigned int len = (unsigned int)textureNames[i].length();
            fout.write( (char*)&len, sizeof(len) );
            fout.write( textureNames[i].c_str(), len );
        }

        count = (unsigned int)meshFileName.length();
        fout.write( (char*)&count, sizeof(count) );
        fout.write( meshFileName.c_str(), count );
    }

    void GDMeshAppearance::Save(const char *fileName, bool bOverWrite = false) const
    {
        ifstream fin(fileName);

        if( fin.bad() || bOverWrite )
        {
            fin.close();
            fin.clear();

            ofstream fout( fileName, ios_base::out | ios_base::binary | ios_base::trunc );

            Write(fout);
            fout.close();
        }
    }

	void GDMeshAppearance::WriteXML(ofstream &fout) const
	{
		fout << "<GDMeshAppearance>\n";
		fout << "\t<Name>" << name.c_str() << "</Name>\n";

		if( flags != 0 )
		{
			fout << "\t<AppearanceFlags>\n"; 
			unsigned int flagCopy = flags;
			unsigned int index = 0;
			while( flagCopy != 0 )
			{
				if( (flagCopy & 1) != 0 )
					fout << "\t\t<" << theAppearanceFlagStrings[index] << ">1" << "</" << theAppearanceFlagStrings[index] << ">\n";
				
				flagCopy >>= 1;
				++index;
			}
			fout << "\t</AppearanceFlags>\n"; 
		}

		fout << "\t<ContextName>" << contextName.c_str() << "</ContextName>\n";
		
		size_t vSize = textureNames.size();
		for( unsigned int i = 0; i < vSize; ++i)
			fout << "\t<TextureName>" << textureNames[i].c_str() << "</TextureName>\n";

		fout << "\t<MeshFileName>" << meshFileName.c_str() << "</MeshFileName>\n";

		fout << "</GDMeshAppearance>\n";
	}

	void GDMeshAppearance::SaveXML(void) const
	{
		string fileName = ContentManager::theContentPath + "/gda_";
		fileName += name;
		fileName += ".xml";

		ofstream fout( fileName.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
		
		fout << "<?xml version=\"1.0\"?>\n";
		
		WriteXML( fout );

		fout.close();		
	}
		
	void GDMeshAppearance::OpenXML(const char* fileName)
	{
		TiXmlDocument doc(fileName);

		if(!doc.LoadFile())
			return;

		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;
		TiXmlHandle hRoot(0);
		
		elementPtr = hDoc.FirstChildElement().Element();
		if( !elementPtr )
			return;

		string rootName = elementPtr->Value();

		if( strcmp(rootName.c_str(), "GDMeshAppearance") != 0 )
			return;

		hRoot = TiXmlHandle(elementPtr);

		elementPtr = hRoot.FirstChild( "Name" ).Element();
		if( elementPtr != 0 )
			name = elementPtr->GetText();

		elementPtr = hRoot.FirstChild( "AppearanceFlags" ).Element();

		flags = 0;
		if( elementPtr != 0 )
		{
			for( int i = 0; i < 5; ++i )
			{
				TiXmlElement* pChildElem = elementPtr->FirstChildElement( theAppearanceFlagStrings[i] );

				if( pChildElem != 0 )
					if( strcmp( "1", pChildElem->GetText() ) == 0 )
						flags |= (1 << i); 
			}
		}

		elementPtr = hRoot.FirstChild( "ContextName" ).Element();

		if( elementPtr != 0 )
			contextName = elementPtr->GetText();

		elementPtr = hRoot.FirstChild( "TextureName" ).Element();

		textureNames.clear();
		while( elementPtr != 0 )
		{
			string fileName = elementPtr->GetText();
			textureNames.push_back( fileName );
			elementPtr = elementPtr->NextSiblingElement( "TextureName" );
		}

		elementPtr = hRoot.FirstChild( "MeshFileName" ).Element();
				
		meshFileName.clear();

		if( elementPtr != 0 )
			meshFileName = elementPtr->GetText();
	}
}