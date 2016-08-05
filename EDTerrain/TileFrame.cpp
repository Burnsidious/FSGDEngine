#include "TileFrame.h"
#include <assert.h>

#include "../EDRendererD3D/Renderer.h"
using namespace EDRendererD3D;

namespace EDTerrain
{
	TileFrame::TileFrame(void)
	{
		tilesPtr = nullptr;
	}

	TileFrame::~TileFrame(void)
	{
		delete [] tilesPtr;
		tilesPtr = nullptr;
	}

	void TileFrame::Initialize(unsigned int width, unsigned int _holeWidth, float _tileSize)
	{
		// No remainders, must have a whole number of tiles to make up frame
		assert((width - _holeWidth) % 2 == 0);

		totalWidth = width;
		holeWidth = _holeWidth;
		tileSize = _tileSize;

		frameWidth = (totalWidth - holeWidth) / 2;
		numTiles = totalWidth*totalWidth - holeWidth*holeWidth;

		CreateVertexBuffer();
	}


	void TileFrame::CreateVertexBuffer(void)
	{
		int index = 0;
		tilesPtr = new TileInstance[numTiles];

		const float halfWidth = 0.5f * (float) totalWidth;

		for (unsigned int y = 0; y < totalWidth; ++y)
		{
			for (unsigned int x = 0; x < totalWidth; ++x)
			{
				if (InFrame(x,y))
				{
					tilesPtr[index].x = tileSize * ((float) x - halfWidth);
					tilesPtr[index].y = tileSize * ((float) y - halfWidth);
					AssignNeighbourSizes(x, y, tilesPtr[index]);
					index++;
				}
			}
		}
		// Sanity check, make sure we initialized all the tiles we think we have
		assert(index == numTiles);

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = tilesPtr;
		D3D11_BUFFER_DESC vbDesc =
		{
			sizeof(TileInstance) * numTiles,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			0,
			0
		};

		HR(Renderer::theDevicePtr->CreateBuffer(&vbDesc, &initData, &vertexBuffer.p));
	}

	bool TileFrame::InFrame(unsigned int x, unsigned int y) const
	{
		assert(x >= 0 && x < totalWidth);
		assert(y >= 0 && y < totalWidth);
		return (x < frameWidth || y < frameWidth ||
			x >= totalWidth - frameWidth || y >= totalWidth - frameWidth);
	}

	void TileFrame::AssignNeighbourSizes(unsigned int x, unsigned int y, 
		TileInstance &tile) const
	{
		tile.posXScale = 1.0f;
		tile.posYScale = 1.0f;
		tile.negXScale = 1.0f;
		tile.negYScale = 1.0f;

		// We always either stay the same size, double, or half our sizes to reduce bad T junctions
		const float innerNeighbourSize = 0.5f;
		const float outerNeighbourSize = 2.0f;

		// Set smaller inner tile adjustments, skip center degenerate frame (no hole)
		if (holeWidth > 0)
		{
			if (y >= frameWidth && y < totalWidth-frameWidth)
			{
				if (x == frameWidth-1)
					tile.posXScale  = innerNeighbourSize;
				if (x == totalWidth - frameWidth)
					tile.negXScale = innerNeighbourSize;
			}
			if (x >= frameWidth && x < totalWidth-frameWidth)
			{
				if (y == frameWidth-1)
					tile.negYScale  = innerNeighbourSize;
				if (y == totalWidth - frameWidth)
					tile.posYScale = innerNeighbourSize;
			}
		}

		// Set larger outer tile adjsutments
		if (x == 0)
			tile.negXScale = outerNeighbourSize;
			//tile.posXScale = outerNeighbourSize;
		if (y == 0)
			//tile.negYScale = outerNeighbourSize;
			tile.posYScale = outerNeighbourSize;
		if (x == totalWidth - 1)
			tile.posXScale  = outerNeighbourSize;
			//tile.negXScale  = outerNeighbourSize;
		if (y == totalWidth - 1)
			//tile.posYScale  = outerNeighbourSize;
			tile.negYScale  = outerNeighbourSize;
	}

}