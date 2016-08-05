#pragma once

#include "GDAttribute.h"

struct GDTriangleAttribute
{
	union
	{
		struct
		{
			float normal_x;
			float normal_y;
			float normal_z;
			float normal_wPad;
		};

		struct
		{
			float red;
			float green;
			float blue;
			float alpha;
		};

		struct
		{
			int vertIndex0;
			int vertIndex1;
			int vertIndex2;
			int	vertIndexPad;
		};

		struct
		{
			int NeighborFace0;
			int NeighborFace1;
			int NeighborFace2;
			int NeighborFacePad;
		};
	};

	GDTriangleAttribute(void) : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f) {}
};
	
struct GDTriangle
{
	GDTriangleAttribute attributes[GDTAT_MAX];
};

