#include "TriNeighborAttribute.h"
#include <maya/MItMeshPolygon.h>
#include <maya/MDagPath.h>
#include <maya/MIntArray.h>

template<>
void CollectTriAttribute<GDTAT_TRINEIGHBORS>(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets)
{
	MDagPath meshPath;
	mfnMesh.getPath(meshPath);
	
	MItMeshPolygon polyIter(meshPath);
	
	int polyIndex = 0;
	for(; !polyIter.isDone(); polyIter.next(), ++polyIndex)
	{
		int numCF;
		polyIter.numConnectedFaces(numCF);
		
		MIntArray CFids;
		polyIter.getConnectedFaces(CFids);
		
		GDTriangleAttribute triAttrib;
		triAttrib.NeighborFace0 = -1;
		triAttrib.NeighborFace1 = -1;
		triAttrib.NeighborFace2 = -1;
		triAttrib.NeighborFacePad = -1;

		if( numCF > 0 )
			triAttrib.NeighborFace0 = CFids[0];
		if( numCF > 1 )
			triAttrib.NeighborFace1 = CFids[1];
		if( numCF > 2 )
			triAttrib.NeighborFace2 = CFids[2];

		pAttributeSets[GDTAT_TRINEIGHBORS].m_Attributes.push_back(triAttrib);
	}
}

template<>
void WriteTriAttributeXML<GDTAT_TRINEIGHBORS>(std::ofstream &fout, GDTriangle &tri)
{
	char buffer[128];
	sprintf_s( buffer, "<Neighbor0>%d</Neighbor0><Neighbor1>%d</Neighbor1><Neighbor2>%d</Neighbor2>\n", tri.attributes[GDTAT_TRINEIGHBORS].NeighborFace0, tri.attributes[GDTAT_TRINEIGHBORS].NeighborFace1, tri.attributes[GDTAT_TRINEIGHBORS].NeighborFace2 );
	fout << buffer;
}

template<>
void WriteTriAttributeBinary<GDTAT_TRINEIGHBORS>(std::ofstream &fout, GDTriangle &tri)
{
	fout.write( (const char*)&tri.attributes[GDTAT_TRINEIGHBORS].NeighborFace0, sizeof(tri.attributes[GDTAT_TRINEIGHBORS].NeighborFace0)*3 );
}