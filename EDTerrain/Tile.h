#pragma once

namespace EDTerrain
{
	// Represents scales of neighboring tiles
	//struct Adjacency
	//{
	//	float negXScale;
	//	float negYScale;
	//	float posXScale;
	//	float posYScale;
	//};

	struct TileInstance
	{
		float x,y;
		float negXScale;
		float negYScale;
		float posXScale;
		float posYScale;
	};
}
