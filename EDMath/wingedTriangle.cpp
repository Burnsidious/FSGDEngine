#include "wingedTriangle.h"

namespace EDMath
{
	struct EdgeEntry
	{
		unsigned int vertexIndex[2];
		int triangleIndex[2];
		int edgeNumber[2];
		EdgeEntry* pNext;

		EdgeEntry(void)
		{
			triangleIndex[0] = triangleIndex[1] = -1;
			edgeNumber[0] = edgeNumber[1] = -1;
			pNext = 0;
		}
	};

	void WingedTriangle::ComputeAdjacencies(WingedTriangle triangles[], unsigned int triCount)
	{
		const unsigned int magic1 = 0x8da6b343;
		const unsigned int magic2 = 0xd8163841;

		int MAX_EDGES = triCount*3;

		EdgeEntry** edgeListHead = new EdgeEntry*[MAX_EDGES];
		EdgeEntry* edgeListEntry = new EdgeEntry[MAX_EDGES];

		for(int i = 0; i < MAX_EDGES; ++i)
			edgeListHead[i] = 0;

		int cnt = 0;

		for(unsigned int i = 0; i < triCount; ++i)
		{
			for(int j = 2, k = 0; k < 3; j = k, k++)
			{
				unsigned int vj = triangles[i].indices[j];
				unsigned int vk = triangles[i].indices[k];

				if( vj > vk )
				{
					unsigned int temp = vj;
					vj = vk;
					vk = temp;
				}

				unsigned int hashKey = (vj*magic1 + vk*magic2) % MAX_EDGES;

				for(EdgeEntry* pEdge = edgeListHead[hashKey]; ; pEdge = pEdge->pNext)
				{
					if( pEdge == 0 )
					{
						edgeListEntry[cnt].vertexIndex[0] = vj;
						edgeListEntry[cnt].vertexIndex[1] = vk;
						edgeListEntry[cnt].triangleIndex[0] = i;
						edgeListEntry[cnt].edgeNumber[0] = j;

						edgeListEntry[cnt].pNext = edgeListHead[hashKey];
						edgeListHead[hashKey] = &edgeListEntry[cnt++];

						break;
					}

					if( pEdge->vertexIndex[0] == vj && pEdge->vertexIndex[1] == vk )
					{
						pEdge->triangleIndex[1] = i;
						pEdge->edgeNumber[1] = j;

						triangles[ pEdge->triangleIndex[0] ].adjacencies[ pEdge->edgeNumber[0] ] = pEdge->triangleIndex[1];
						triangles[ pEdge->triangleIndex[1] ].adjacencies[ pEdge->edgeNumber[1] ] = pEdge->triangleIndex[0];

						break;
					}
				}
			}
		}

		delete [] edgeListHead;
		delete [] edgeListEntry;
	}
}