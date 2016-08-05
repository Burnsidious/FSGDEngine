#pragma once

#include "indexedTriangle.h"

namespace EDMath
{
	struct WingedTriangle : IndexedTriangle
	{
		union
		{
			int adjacencies[3];

			struct
			{
				int a0;
				int a1;
				int a2;
			};
		};

		WingedTriangle(void) : a0(-1), a1(-1), a2(-1) {}		

		static void ComputeAdjacencies(WingedTriangle triangles[], unsigned int count);
	};
}