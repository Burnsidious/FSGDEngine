#include "BitangentAttribute.h"

template<>
int AttributeIndex<GDVAT_BITANGENTS>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName)
{
	MStatus result;
	int index = polyIt.tangentIndex( polyVertNum, &result );

	if( result == MS::kFailure )
		return -1;

	return index;
}

template<>
void CollectAttribute<GDVAT_BITANGENTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	MFloatVectorArray bitangents;
	mfnMesh.getBinormals( bitangents );

	pAttributeSets[GDVAT_BITANGENTS].m_Attributes.resize( bitangents.length() );

	for(unsigned int i = 0; i < bitangents.length(); ++i)
	{
		pAttributeSets[GDVAT_BITANGENTS].m_Attributes[i].x = bitangents[i].x;
		pAttributeSets[GDVAT_BITANGENTS].m_Attributes[i].y = bitangents[i].y;
		pAttributeSets[GDVAT_BITANGENTS].m_Attributes[i].z = bitangents[i].z;
	}
}