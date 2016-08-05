#include "TriNormalAttribute.h"
#include <maya/MVector.h>

template<>
void CollectTriAttribute<GDTAT_TRINORMS>(const MFnMesh& mfnMesh, GDTriangleAttributeSet* pAttributeSets)
{
	int numPolys = mfnMesh.numPolygons();

	pAttributeSets[GDTAT_TRINORMS].m_Attributes.resize( numPolys );

	for(int i = 0; i < numPolys; ++i)
	{
		MVector normal;
		mfnMesh.getPolygonNormal(i, normal);
		pAttributeSets[GDTAT_TRINORMS].m_Attributes[i].normal_x = (float)normal.x;
		pAttributeSets[GDTAT_TRINORMS].m_Attributes[i].normal_y = (float)normal.y;
		pAttributeSets[GDTAT_TRINORMS].m_Attributes[i].normal_z = (float)normal.z * -1.0f;
	}
}
