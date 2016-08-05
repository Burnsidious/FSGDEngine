#pragma once

#include "GDMeshAttribute.h"
namespace EDUtilities
{
    // GDMesh
    // Basic implementation by Matt Norris (mnorris@fullsail.com)
    // 
    // Note:	I wrote this quickly and lazily. I offer no promises that it will
    //			be efficient, safe, and/or not destroy your computer and loved ones.
    class GDMesh
    {
    private:
        // The mesh name
        std::string name;

        // The number of unique vertices in the mesh
        int numVerts;

        // The indices for the mesh
        std::vector<int> indices;

        // The array of attributes for the mesh
        std::vector<GDMeshAttribute> attributes;

        // Unload - Releases all dynamic memory of the mesh
        void Unload(void);

        // Read - Reads a mesh from an input file stream
        void Read(std::ifstream &fin);

        // Write - Writes a mesh to an output file stream
        void Write(std::ofstream &fout);

    public:
        // Default constructor
        GDMesh(void);

        // Creates a mesh from a ".gdm" file on disk
        GDMesh(const char* FileName);

        // GetNameLen - Get the length of the mesh name
        size_t GetNameLen(void) const { return name.length(); }
        // GetName - Get the name of the mesh
        const char* GetName(void) const { return name.c_str(); }
        // SetName - Set the name for the mesh, input string must be null terminated.
        void SetName(const char* Name);

        // GetNumIndices - Get the number of indices in the mesh
        size_t GetNumIndices(void) const { return indices.size(); }
        // GetIndices - Get the indices of the mesh
        const int* GetIndices(void) const 
        { 
            return (indices.empty() ? 0 : &indices[0]);
        }

        // SetIndices - Set the indices of the mesh. Copies the input data, does not retain the input pointer.
        void SetIndices(const int* _indices, int _numIndices);

        // GetNumVerts - Get the number of verts in the mesh
        int GetNumVerts(void) const { return numVerts; }
        // SetNumVerts - Set the number of verts in the mesh
        void SetNumVerts(int _numVerts);

        // GetNumAttributeSets - Gets the number of attribute sets in the mesh
        size_t GetNumAttributeSets(void) const { return attributes.size(); }
        // GetAttributes - Get the array of attribute sets
        const GDMeshAttribute* GetAttributes(void) const;

        // SetAttributes - Sets the attribute sets. Copies the input data, does not retain the input pointer.
        void SetAttributes(const GDMeshAttribute* _attributes, int numAttributeSets);
        void SetAttributes(const std::vector<GDMeshAttribute>& attribs){ attributes = attribs; }

        // GetAttribute - Get an attribute set by name, return null if not found.
        const GDMeshAttribute* GetAttribute(const char* attributeName) const;
        // SetAttribute - Sets an attribute set by name, adds it to the array of attribute sets if not found.
        void SetAttribute(const char* attributeName, const char* attributeData, int numBytes);
        // RemoveAttribute - Removes an attribute set from the array of attribute sets
        void RemoveAttribute(const char* attributeName);

        // Load - Load a mesh from file. Returns null if file not found.
        static GDMesh* Load(const char* fileName);

        // Save - Saves a mesh to file. If file exists it will not be overwritten unless OverWrite is true
        void Save(const char* fileName, bool overWrite);

		void SaveXML(void) const;
		void WriteXML(std::ofstream &fout) const;
		void OpenXML(const char* fileNamePtr);
    };
}