#pragma once

#include <d3d11.h>
#include <atlbase.h> 

#include "Tile.h"


namespace EDTerrain
{
	class TileFrame
	{
	public:
		TileFrame(void);
		~TileFrame(void);

		CComPtr<ID3D11Buffer> &GetVertexBuffer(void) { return vertexBuffer; }
		void Initialize(unsigned int width, unsigned int _holeWidth, float _tileSize);

		unsigned int GetNumTiles(void) { return numTiles; }

	private:
		// private variable
		CComPtr<ID3D11Buffer> vertexBuffer;

		unsigned int totalWidth;
		unsigned int holeWidth;
		unsigned int frameWidth;
		unsigned int numTiles;
		float tileSize;

		TileInstance *tilesPtr;

		// private methods
		void CreateVertexBuffer(void);
		bool InFrame(unsigned int x, unsigned int y) const;
		void AssignNeighbourSizes(unsigned int x, unsigned int y, TileInstance &tile) const;

		// hiden methods
	};
}