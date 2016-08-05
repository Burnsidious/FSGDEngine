#pragma once

#include "EDCollision.h"
#include "sphere.h"
#include "../EDMath/indexedTriangle.h"
#include <list>

namespace EDCollision
{
	class HeightField
	{
		float* heights;
		unsigned int length;
		unsigned int width;
		float mapScale;

		HeightField(const HeightField&){}
		HeightField& operator=(const HeightField&){ return *this; }

		void Deinitialize(void);

		void GetIndices( int indices[2][3], int tileX, int tileY );

		void GetTileIndices( int& tileX, int& tileY, Float3 pos )const;

	public:

		HeightField(void) : heights(0), length(0), width(0), mapScale(0.0f) {}

		~HeightField();

		void Initialize( unsigned char* bytes, unsigned int length, unsigned int width, float heightScale, float heightFieldOffset, float mapScale, unsigned int byteOffset = 0 );

		void GetTriangles(const EDMath::Aabb& aabb, std::list<Float3>& verts);

		template<typename Func>
		void ForEachTriangle(const EDMath::Aabb& aabb, Func func);
	};


	template<typename Func>
	void HeightField::ForEachTriangle(const EDMath::Aabb& aabb, Func func)
	{
		int minTileX;
		int minTileY;
		int maxTileX;
		int maxTileY;

		GetTileIndices( minTileX, minTileY, aabb.Min() );
		GetTileIndices( maxTileX, maxTileY, aabb.Max() );

		for(int y = minTileY; y < maxTileY+1; ++y )
		{
			for(int x = minTileX; x < maxTileX+1; ++x )
			{
				int mapX = x + (width >> 1);

				mapX %= width;

				if( mapX < 0 )
					mapX += width;

				int mapY = y + (length >> 1);
				
				mapY %= length;
				
				if( mapY < 0 )
					mapY += length;

				int deltaX = x - mapX;
				int deltaY = y - mapY;

				int indices[2][3];

				GetIndices( indices, x, y );

				Float3 point[3];
				point[0].x = x * mapScale;
				point[0].y = heights[ indices[0][0] ];
				point[0].z = y * mapScale;
				
				point[1].x = x * mapScale;
				point[1].y = heights[ indices[0][1] ];
				point[1].z = (y+1) * mapScale;

				point[2].x = (x+1) * mapScale;
				point[2].y = heights[  indices[0][2] ];
				point[2].z = (y+1) * mapScale;

				func( point );

				point[0].x = x * mapScale;
				point[0].y = heights[ indices[1][0] ];
				point[0].z = y * mapScale;

				point[1].x = (x+1) * mapScale;
				point[1].y = heights[  indices[1][1] ];
				point[1].z = (y+1) * mapScale;

				point[2].x = (x+1) * mapScale;
				point[2].y = heights[ indices[1][2] ];
				point[2].z = y * mapScale;

				func( point );
			}
		}
	}

}