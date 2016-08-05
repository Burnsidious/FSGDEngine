
#include "../EDUtilities/bmpfile.h"

using std::ifstream;
using std::ios_base;
using std::ofstream;
using std::pair;
using std::string;
namespace EDUtilities
{
    BMPFile::BMPFile(void)
    {
        memset( &this->fileHeader, 0, sizeof(BMPFileHeader) );
        memset( &this->imageHeader, 0, sizeof(BMPImageHeader) );

        colorTablePtr = NULL;
        pixelDataPtr = NULL;
        fileNamePtr = NULL;
    }

    BMPFile::BMPFile(const char *fileName)
    {
        memset( &this->fileHeader, 0, sizeof(BMPFileHeader) );
        memset( &this->imageHeader, 0, sizeof(BMPImageHeader) );

        colorTablePtr = NULL;
        pixelDataPtr = NULL;
        fileNamePtr = NULL;

        LoadFromFile( fileName );
    }

    BMPFile::~BMPFile(void)
    {
		delete []colorTablePtr;
		delete []pixelDataPtr;
		delete []fileNamePtr;
    }

    bool BMPFile::LoadFromFile( const char *fileName, bool bOverWrite )
    {
        ifstream bmpFile;
        BMPFileHeader tempFileHeader;
        BMPImageHeader tempImageHeader;
        int headerOffset;

        bmpFile.open( fileName, std::ios_base::binary | std::ios_base::in );

        if( bmpFile.is_open() )
            bmpFile.read( tempFileHeader.type, sizeof(char)*2 );
        else
            return false;

        if( tempFileHeader.type[0] != 'B' && tempFileHeader.type[1] != 'M' )
        {
            bmpFile.close();
            return false;
        }

        bmpFile.read( (char*)&tempFileHeader.size, sizeof( size_t ) );
        bmpFile.read( (char*)&tempFileHeader.reserved1, sizeof( short ) );
        bmpFile.read( (char*)&tempFileHeader.reserved2, sizeof( short ) );
        bmpFile.read( (char*)&tempFileHeader.offbits, sizeof( size_t ) );

        bmpFile.read( (char*)&tempImageHeader.size, sizeof( size_t ) );

        if( tempImageHeader.size < 40 )
        {
            bmpFile.close();
            return false;
        }
        else
            headerOffset = (int)tempImageHeader.size - 40;

        bmpFile.read( (char*)&tempImageHeader.width, sizeof(size_t) );
        bmpFile.read( (char*)&tempImageHeader.height, sizeof(size_t) );
        bmpFile.read( (char*)&tempImageHeader.planes, sizeof(short) );

        if( tempImageHeader.planes != 1 )
        {
            bmpFile.close();
            return false;
        }

        bmpFile.read( (char*)&tempImageHeader.bitCount, sizeof(short) );

        if( tempImageHeader.bitCount != 8 && tempImageHeader.bitCount != 24 )
        {
            bmpFile.close();
            return false;
        }

        bmpFile.read( (char*)&tempImageHeader.compression, sizeof( size_t ) );

        if( tempImageHeader.compression != 0 )
        {
            bmpFile.close();
            return false;
        }

        if( fileNamePtr != NULL )
        {
            if( bOverWrite != true )
            {
                bmpFile.close();
                return false;
            }
            else 
            {
				delete []colorTablePtr;
				delete []pixelDataPtr;
				delete []fileNamePtr;

                memset( &this->fileHeader, 0, sizeof(BMPFileHeader) );
                memset( &this->imageHeader, 0, sizeof(BMPImageHeader) );

                colorTablePtr = NULL;
                pixelDataPtr = NULL;
                fileNamePtr = NULL;
            }
        }

        bmpFile.read( (char*)&tempImageHeader.sizeImage, sizeof( size_t ) );
        bmpFile.read( (char*)&tempImageHeader.xPelsPerMeter, sizeof( size_t ) );
        bmpFile.read( (char*)&tempImageHeader.yPelsPerMeter, sizeof( size_t ) );
        bmpFile.read( (char*)&tempImageHeader.clrUsed, sizeof( size_t ) );
        bmpFile.read( (char*)&tempImageHeader.clrImportant, sizeof( size_t ) );

        bmpFile.seekg( headerOffset, ios_base::cur );

		fileNamePtr = new char[strlen(fileName)+1];
        strcpy_s( fileNamePtr, strlen(fileName) + 1, fileName );
        //strcpy( fileNamePtr, fileName );

        if( tempImageHeader.bitCount == 8 )
        {
            int pad = (4 - ((int)tempImageHeader.width & 3) ) & 3;

			colorTablePtr = new BMPPaletteColor[256];
            bmpFile.read( (char*)colorTablePtr, sizeof(size_t)*256 );

			pixelDataPtr = new unsigned char[tempImageHeader.width * tempImageHeader.height];

            for( unsigned int i = 0; i < tempImageHeader.height; ++i )
            {
                bmpFile.read( (char*)&pixelDataPtr[tempImageHeader.width * i], (std::streamsize)tempImageHeader.width );
                bmpFile.seekg( pad, ios_base::cur );
            }
        }
        else
        {
            int pad = (4 - (((int)tempImageHeader.width * 3) & 3) ) & 3;

			pixelDataPtr = new unsigned char[tempImageHeader.width * 3 * tempImageHeader.height];

            for( unsigned int i = 0; i < tempImageHeader.height; ++i )
            {
                bmpFile.read( (char*)&pixelDataPtr[tempImageHeader.width * 3 * i], (std::streamsize)tempImageHeader.width * 3 );
                bmpFile.seekg( pad, ios_base::cur );
            }
        }

        fileHeader = tempFileHeader;
        imageHeader = tempImageHeader;

        bmpFile.close();
        return true;
    }
}