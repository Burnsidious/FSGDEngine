#pragma once


namespace EDUtilities
{

    union BMPPaletteColor
    {
        struct
        {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char unused;
        };

        size_t color;
    };

    class BMPFile
    {
        friend class BMPHeightMap;

    private:
        BMPFile(const BMPFile &){}
        BMPFile &operator=(const BMPFile &){ return *this; }

    protected:
        // File format definitions
        struct BMPFileHeader
        {
            char type[2];
            size_t size;
            short reserved1;
            short reserved2;
            size_t offbits;
        };

        struct BMPImageHeader
        {
            size_t size;
            size_t width;
            size_t height;
            short planes;
            short bitCount;
            size_t compression;
            size_t sizeImage;
            size_t xPelsPerMeter;
            size_t yPelsPerMeter;
            size_t clrUsed;
            size_t clrImportant;		
        };

        // Members
        BMPFileHeader fileHeader;
        BMPImageHeader imageHeader;

        BMPPaletteColor *colorTablePtr;
        unsigned char *pixelDataPtr;

        char *fileNamePtr;

    public:

        BMPFile(void);
        BMPFile( const char *fileName );
        virtual ~BMPFile(void);

        bool IsValid(void){ return fileNamePtr != NULL; }
        bool IsPalettized(void){ return colorTablePtr != NULL; }
        size_t GetWidth(void){ return imageHeader.width; }
        size_t GetHeight(void){ return imageHeader.height; }
        const unsigned char *GetPixelData(void){ return pixelDataPtr; }
        const BMPPaletteColor *GetPaletteColors(void){ return colorTablePtr; }

        bool LoadFromFile( const char *fileName, bool bOverWrite = false );
    };
}