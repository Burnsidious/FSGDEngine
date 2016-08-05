#pragma once


namespace EDUtilities
{

    enum GDTextureDimension { GD_TEXTURE_1D = 1, GD_TEXTURE_2D = 2, GD_TEXTURE_3D = 3, GD_TEXTURE_CUBE_MAP = 4 };

    class GDTextureObject
    {
        std::string name;
        int dimension;
        int textureFunc;
        int wrapMode[3];
        float borderColor[4];
        std::vector< std::string > textureFileNames;


		void WriteXML(std::ofstream &fout) const;

    public:

        GDTextureObject(void)
        {
            dimension = 0;
            textureFunc = 0;
            wrapMode[0] = wrapMode[1] = wrapMode[2] = 0;
            borderColor[0] = borderColor[1] = borderColor[2] = borderColor[3] = 0.0f;
        }

        GDTextureObject(const char* fileName);
        GDTextureObject(std::ifstream &fin);

        const char* GetName(void) const;
        void SetName(const char* _name);

        int GetDimension(void) const;
        void SetDimension(int _dimension);

        int GetTextureFunc(void) const;
        void SetTextureFunc(int _textureFunc);

        const int* GetWrapModes(void) const;
        void SetWrapModes(const int modes[3]);
        void SetWrapMode_S(int mode_S);
        void SetWrapMode_T(int mode_T);
        void SetWrapMode_R(int mode_R);

        const float* GetBorderColor(void) const;
        void SetBorderColor(const float color[4]);

        const std::vector< std::string >& GetTextureFileNames(void) const;
        void SetTextureFileNames( const std::vector< std::string >& names );

        void Read(std::ifstream &fin);
        void Write(std::ofstream &fout) const;

        void Save(const char* fileName, bool bOverWrite);
		void SaveXML(void) const;
		void OpenXML(const char* fileName);
    };
}