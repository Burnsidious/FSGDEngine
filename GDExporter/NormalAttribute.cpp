#include "NormalAttribute.h"

template<>
void CollectAttribute<GDVAT_NORMALS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	MStatus result;
	const float* pNormals = mfnMesh.getRawNormals(&result);
	
	if( result != MS::kSuccess )
		return;
	
	unsigned int numNorms = mfnMesh.numNormals();

	if( numNorms == 0 )
		return;

	if( numNorms > pAttributeSets[GDVAT_NORMALS].m_Attributes.size() )
		pAttributeSets[GDVAT_NORMALS].m_Attributes.resize( numNorms );

	for(unsigned int i = 0; i < numNorms; ++i)
	{
		memcpy_s( pAttributeSets[GDVAT_NORMALS].m_Attributes[i].m_v, sizeof(GDVertexAttribute), &pNormals[i*3], sizeof(float)*3 );
		// Convert to left-handed coordinate system.
		pAttributeSets[GDVAT_NORMALS].m_Attributes[i].z *= -1.0f;
	}
}

template<>
int AttributeIndex<GDVAT_NORMALS>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName)
{
	MStatus result;
	int index = polyIt.normalIndex( polyVertNum, &result );

	if( result == MS::kFailure )
		return -1;

	return index;	
}