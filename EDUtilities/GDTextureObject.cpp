#include "GDTextureObject.h"
#include "..\tinyxml\tinyxml.h"
#include "../EDUtilities/ContentManager.h"

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;

namespace EDUtilities
{
    GDTextureObject::GDTextureObject(const char* fileName)
    {
        ifstream fin(fileName, std::ios_base::in | std::ios_base::binary);

        if( fin.is_open() )
        {
            Read(fin);
            fin.close();
        }
    }

    GDTextureObject::GDTextureObject(ifstream &fin)
    {
        Read(fin);
    }

    const char* GDTextureObject::GetName(void) const
    {
        return name.c_str();
    }

    void GDTextureObject::SetName(const char* _name)
    {
        name = _name;
    }

    int GDTextureObject::GetDimension(void) const
    {
        return dimension;
    }

    void GDTextureObject::SetDimension(int _dimension)
    {
        dimension = _dimension;
    }

    int GDTextureObject::GetTextureFunc(void) const
    {
        return textureFunc;
    }

    void GDTextureObject::SetTextureFunc(int _textureFunc)
    {
        textureFunc = _textureFunc;
    }

    const int* GDTextureObject::GetWrapModes(void) const
    {
        return wrapMode;
    }

    void GDTextureObject::SetWrapModes(const int modes[3])
    {
        wrapMode[0] = modes[0];
        wrapMode[1] = modes[1];
        wrapMode[2] = modes[2];
    }

    void GDTextureObject::SetWrapMode_S(int mode_S)
    {
        wrapMode[0] = mode_S;
    }

    void GDTextureObject::SetWrapMode_T(int mode_T)
    {
        wrapMode[1] = mode_T;
    }

    void GDTextureObject::SetWrapMode_R(int mode_R)
    {
        wrapMode[2] = mode_R;
    }

    const float* GDTextureObject::GetBorderColor(void) const
    {
        return borderColor;
    }

    void GDTextureObject::SetBorderColor(const float color[4])
    {
        borderColor[0] = color[0];
        borderColor[1] = color[1];
        borderColor[2] = color[2];
        borderColor[3] = color[3];
    }

    const vector< string >& GDTextureObject::GetTextureFileNames(void) const
    {
        return textureFileNames;
    }

    void GDTextureObject::SetTextureFileNames( const vector< string >& names )
    {
        textureFileNames = names;
    }

    void GDTextureObject::Read(ifstream &fin)
    {
        int count;
        fin.read( (char*)&count, sizeof(count) );
		char *_name = new char[count+1];
        _name[count] = 0;
        fin.read( _name, count );
        name = _name;
		delete []_name;

        fin.read( (char*)&dimension, sizeof(dimension) );
        fin.read( (char*)&textureFunc, sizeof(textureFunc) );
        fin.read( (char*)wrapMode, sizeof(wrapMode) );
        fin.read( (char*)borderColor, sizeof(borderColor) );

        if( dimension == GD_TEXTURE_CUBE_MAP )
            count = 6;
        else
            count = 1;

        textureFileNames.resize(count);

        for( int i = 0; i < count; ++i)
        {
            int namelen;
            fin.read( (char*)&namelen, sizeof(namelen) );
			_name = new char[namelen+1];
            _name[namelen] = 0;
            fin.read( _name, namelen );
            textureFileNames[i] = _name;
			delete []_name;
        }
    }

    void GDTextureObject::Write(ofstream &fout) const
    {
        unsigned int namelen = (unsigned int)name.length();
        fout.write( (char*)&namelen, sizeof(namelen) );
        fout.write( name.c_str(), namelen );

        fout.write( (char*)&dimension, sizeof(dimension) );
        fout.write( (char*)&textureFunc, sizeof(textureFunc) );
        fout.write( (char*)wrapMode, sizeof(wrapMode) );
        fout.write( (char*)borderColor, sizeof(borderColor) );

        size_t count = textureFileNames.size();
        for( size_t i = 0; i < count; ++i )
        {
            namelen = (unsigned int)textureFileNames[i].length();
            fout.write( (char*)&namelen, sizeof(namelen) );
            fout.write( textureFileNames[i].c_str(), namelen );
        }
    }

    void GDTextureObject::WriteXML(ofstream &fout) const
    {
		const char* dims[] = { "0", "1D", "2D", "3D", "CubeMap" };
		const char* funcs[] = { "Modulate", "Decal", "Blend", "Replace" };
		const char* wraps[] = { "Clamp", "Repeat", "ClampToEdge" };

		char colors[4][4];
		for(int i = 0; i < 4; ++i)
		{
			int val = (int)(255 * borderColor[i]);
			if( val < 0 )
				val = 0;
			if( val > 255 )
				val = 255;

			sprintf_s( colors[i], "%d", val ); 
		}

		fout << "<GDTextureObject>\n";
		fout << "\t<Name>" << name.c_str() << "</Name>\n";
		fout << "\t<Dimensions>" << dims[dimension] << "</Dimensions>\n";
		fout << "\t<TextureFunc>" << funcs[textureFunc] << "</TextureFunc>\n";
		fout << "\t<WrapModes>\n";
		fout << "\t\t<S>" << wraps[wrapMode[0]] << "</S>\n";
		fout << "\t\t<T>" << wraps[wrapMode[0]] << "</T>\n";
		fout << "\t\t<R>" << wraps[wrapMode[0]] << "</R>\n";
		fout << "\t</WrapModes>\n";
		fout << "\t<BorderColor>\n";
		fout << "\t\t<R>" << colors[0] << "</R>\n";
		fout << "\t\t<G>" << colors[1] << "</G>\n";
		fout << "\t\t<B>" << colors[2] << "</B>\n";
		fout << "\t\t<A>" << colors[3] << "</A>\n";
		fout << "\t</BorderColor>\n";
		size_t vSize = textureFileNames.size();
		for( unsigned int i = 0; i < vSize; ++i )
			fout << "\t<TextureFileName>" << textureFileNames[i].c_str() << "</TextureFileName>\n";

		fout << "</GDTextureObject>\n";
    }

    void GDTextureObject::Save(const char* fileName, bool bOverWrite = false)
    {
        ifstream fin(fileName);

        if( fin.bad() || bOverWrite )
        {
            fin.close();
            fin.clear();

            ofstream fout( fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

            Write(fout);
            fout.close();
        }
    }

	void GDTextureObject::SaveXML(void) const
	{
		string fileName = ContentManager::theContentPath + "/gdt_";
		fileName += name;
		fileName += ".xml";

		ofstream fout( fileName.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
		
		fout << "<?xml version=\"1.0\"?>\n";
		
		WriteXML( fout );

		fout.close();
	}

	void GDTextureObject::OpenXML(const char* fileName)
	{
		const char* dims[] = { "0", "1D", "2D", "3D", "CubeMap" };
		const char* funcs[] = { "Modulate", "Decal", "Blend", "Replace" };
		const char* wraps[] = { "Clamp", "Repeat", "ClampToEdge" };
		const char* str[] = { "S", "T", "R" };
		const char* rgba[] = { "R", "G", "B", "A" };

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

		if( strcmp(rootName.c_str(), "GDTextureObject") != 0 )
			return;

		hRoot = TiXmlHandle(elementPtr);

		elementPtr = hRoot.FirstChild( "Name" ).Element();
		if( elementPtr != 0 )
			name = elementPtr->GetText();

		elementPtr = hRoot.FirstChild( "Dimensions" ).Element();
		if( elementPtr != 0 )
		{
			const char* pVal = elementPtr->GetText();

			dimension = 0;
			for(int i = 0; i < 5; ++i)
				if( strcmp( pVal, dims[i] ) == 0 )
				{
					dimension = i;
					break;
				}
		}

		elementPtr = hRoot.FirstChild( "TextureFunc" ).Element();
		if( elementPtr != 0 )
		{
			const char* pVal = elementPtr->GetText();
			
			textureFunc = 0;
			for( int i = 0; i < 4; ++i )
				if( strcmp( pVal, funcs[i] ) == 0 )
				{
					textureFunc = i;
					break;
				}
		}

		elementPtr = hRoot.FirstChild( "WrapModes" ).Element();

		if( elementPtr != 0 )
		{
			for( int dimension = 0; dimension < 3; ++dimension )
			{
				TiXmlElement* pChildElem = elementPtr->FirstChildElement( str[dimension] );

				if( pChildElem != 0 )
				{
					const char* pVal = pChildElem->GetText();
					
					wrapMode[dimension] = 0;

					for( int i = 0; i < 3; ++i )
						if( strcmp( pVal, wraps[i] ) == 0 )
						{
							wrapMode[dimension] = i;
							break;
						}
				}
			}
		}

		elementPtr = hRoot.FirstChild( "BorderColor" ).Element();

		if( elementPtr != 0 )
		{
			borderColor[0] = borderColor[1] = borderColor[2] = 0.0f;
			borderColor[3] = 1.0f;

			for( int colorindex = 0; colorindex < 4; ++colorindex )
			{
				TiXmlElement* pChildElem = elementPtr->FirstChildElement( rgba[colorindex] );

				if( pChildElem != 0 )
				{
					const char* pVal = pChildElem->GetText();
					
					int val = atoi( pVal );
					borderColor[colorindex] = ((float)val) / 255.0f; 
					
					if( borderColor[colorindex] < 0.0f )
						borderColor[colorindex] = 0.0f;
					else if( borderColor[colorindex] > 1.0f )
						borderColor[colorindex] = 1.0f;
				}
			}
		}

		elementPtr = hRoot.FirstChild( "TextureFileName" ).Element();
		textureFileNames.clear();
		while( elementPtr != 0 )
		{
			string textureFileName = elementPtr->GetText();
			textureFileNames.push_back( textureFileName );
			elementPtr = elementPtr->NextSiblingElement( "TextureFileName" );
		}
	}
}