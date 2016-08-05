#pragma once

namespace EDMath
{
	struct IndexedTriangle
	{
		union
		{
			unsigned int indices[3];

			struct
			{
				unsigned int i0;
				unsigned int i1;
				unsigned int i2;
			};
		};
	};
}