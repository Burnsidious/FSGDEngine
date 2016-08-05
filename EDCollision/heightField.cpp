#include "heightField.h"
#include <assert.h>

#ifndef isPowerOfTwo
#define isPowerOfTwo(x) ((x != 0) && !(x & (x - 1)))
#endif

namespace EDCollision
{
	HeightField::~HeightField()
	{
		Deinitialize();	
	}

	void HeightField::Deinitialize(void)
	{
		delete [] heights;
		length = 0;
		width = 0;
		mapScale = 0.0f;
	}

	void HeightField::Initialize( unsigned char* bytes, unsigned int _length, unsigned int _width, float heightScale, float heightFieldOffset, float _mapScale, unsigned int byteOffset )
	{
		Deinitialize();

		assert( bytes != nullptr );
		assert( isPowerOfTwo(_length) );
		assert( isPowerOfTwo(_width) );
		assert( _mapScale > 0.0f );
		
		length = _length;
		width = _width;
		mapScale = _mapScale;

		heights = new float[length*width];
		
		byteOffset = max(1, byteOffset);

		for(unsigned int y = 0; y < length; ++y)
		{
			for(unsigned int x = 0; x < width; ++x)
			{
				float fval = bytes[ (y*width+x) * byteOffset ] / 255.0f;

				heights[ y*width + x ] = (fval + heightFieldOffset) * heightScale;
			}
		}
	}

	void HeightField::GetTileIndices( int& tileX, int& tileY, Float3 pos )const
	{
		//pos *= (1.0f / (mapScale*2.0f));
		
		pos.x += mapScale * 0.5f;
		pos.z += mapScale * 0.5f;

		pos *= (1.0f / mapScale);

		pos.x *= width;
		pos.z *= length;

		tileX = int( pos.x < 0.0f ? pos.x - 1.0f : pos.x );
		
		tileY = int( pos.z < 0.0f ? pos.z - 1.0f : pos.z );
	}

	void HeightField::GetIndices( int indices[2][3], int x, int y )
	{
		#define MAP(x,y) (((y) % length) * width + ((x) % width))

		indices[0][0] = MAP(x, y);
		indices[0][1] = MAP(x, y+1);
		indices[0][2] = MAP(x+1, y+1);
		indices[1][0] = indices[0][0];
		indices[1][1] = indices[0][2];
		indices[1][2] = MAP(x+1, y);

		#undef MAP
	}
}