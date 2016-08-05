#include "PointAttribute.h"

template<>
void CollectAttribute<GDVAT_POINTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	MStatus result;
	const float* pPoints = mfnMesh.getRawPoints(&result);
	
	if( result != MS::kSuccess )
		return;
	
	unsigned int numVerts = mfnMesh.numVertices();

	if( numVerts == 0 )
		return;

	if( numVerts > pAttributeSets[GDVAT_POINTS].m_Attributes.size() )
		pAttributeSets[GDVAT_POINTS].m_Attributes.resize( numVerts );

	for(unsigned int i = 0; i < numVerts; ++i)
	{
		memcpy_s( pAttributeSets[GDVAT_POINTS].m_Attributes[i].m_v, sizeof(GDVertexAttribute), &pPoints[i*3], sizeof(float)*3 );
		// Convert to left-handed coordinate system.
		pAttributeSets[GDVAT_POINTS].m_Attributes[i].z *= -1.0f;
	}
}