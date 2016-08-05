#pragma once


namespace EDUtilities
{
    enum GDMeshAppearanceFlag{ GDA_INDEXED = 1, GDA_TEXTURED = 2, GDA_LIT = 4, GDA_ALPHA = 8, GDA_CUBEMAP = 16 };

    class GDMeshAppearance
    {
        std::string name;
        unsigned int flags;
        std::string contextName;
        std::vector< std::string > textureNames;
        std::string meshFileName;

		static const char* theAppearanceFlagStrings[];
    public:

		GDMeshAppearance(void);
        GDMeshAppearance(const char* fileName);
        GDMeshAppearance(std::ifstream &fin);

        const char* GetName(void) const;
        void SetName(const char* name);

        unsigned int GetAppearanceFlags(void) const;
        void SetAppearanceFlags(unsigned int flags);

        const char* GetContextName(void) const;
        void SetContextName(const char* _contextName);

        const std::vector< std::string >& GetTextureFileNames(void) const;
        void SetTextureFileNames(std::vector< std::string >& names);
        void AddTextureFileName(const char* _textureName);
        void RemoveTextureFileName(const char* _textureName);

        const char* GetMeshFileName(void) const;
        void SetMeshFileName(const char* _meshFileName);

        void Read(std::ifstream &fin);
        void Write(std::ofstream &fout) const;

        void Save(const char *fileName, bool bOverWrite) const;

		void WriteXML(std::ofstream &fout) const;
		void SaveXML(void) const;
		
		void OpenXML(const char* fileName);
    };
}