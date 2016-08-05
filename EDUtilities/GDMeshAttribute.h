#pragma once

#ifndef GDMESH_ATTRIB_NAMELEN
#define GDMESH_ATTRIB_NAMELEN 16
#endif

#include "../tinyxml/tinyxml.h"


namespace EDUtilities
{
    // GDMeshAttribute
    // Used by GDMesh class.
    // Represents a set of attributes for a mesh.
    // Ex: Array of vertex normals
    class GDMeshAttribute
    {
    private:
        // Name for the attribute
        // Names describe the attribute and its format
        // Ex: "Position_3f" - Positions as 3 floats each
        //	   "TexCoord0_2f" - Texcoord set 0 as 2 floats each
        //	   "Normal_3f" - Normals as 3 floats each
        // Names are case insensitive
        std::string attributeName;

        // The array/set of attributes as an array of BYTES
        std::vector<char> attributeData;

    public:
        // GetAttributeName - Gets the name of the attribute
        const char* GetAttributeName(void) const;
        // GetAttributeData - Gets the data of the attribute
        const char* GetAttributeData(void) const;
        // GetAttributeByteSize - Gets the size in bytes of the attribute set
        size_t GetAttributeByteSize(void);

        // SetAttribute - Set the attribute, makes a deep copy of the attribute data
        void SetAttribute( const char* _attributeName, const char* _attributeData, int numBytes);

        // Read - Read an attribute set from an input file stream
        void Read(std::ifstream &fin);
        // Write - Writes an attribute set to an output file stream
        void Write(std::ofstream &fout);

		void SaveXML(void) const;
		void WriteXML(std::ofstream &fout) const;
		void OpenXML(const char* fileName);
		void ReadXML(TiXmlElement *elementPtr);
    };
}