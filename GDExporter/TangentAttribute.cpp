#include "TangentAttribute.h"

template<>
int AttributeIndex<GDVAT_TANGENTS>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName)
{
	MStatus result;
	int index = polyIt.tangentIndex( polyVertNum, &result );

	if( result == MS::kFailure )
		return -1;

	return index;
}

template<>
void CollectAttribute<GDVAT_TANGENTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	MFloatVectorArray tangents;
	mfnMesh.getTangents( tangents );

	pAttributeSets[GDVAT_TANGENTS].m_Attributes.resize( tangents.length() );

	for(unsigned int i = 0; i < tangents.length(); ++i)
	{
		pAttributeSets[GDVAT_TANGENTS].m_Attributes[i].x = tangents[i].x;
		pAttributeSets[GDVAT_TANGENTS].m_Attributes[i].y = tangents[i].y;
		pAttributeSets[GDVAT_TANGENTS].m_Attributes[i].z = tangents[i].z;
	}
}

template<>
void WriteAttributeXML<GDVAT_TANGENTS>(std::ofstream &fout, GDVertex &vert)
{
	char buffer[128];
	sprintf_s( buffer, "<x>%f</x><y>%f</y><z>%f</z><w>%f</w>\n", vert.attributes[GDVAT_TANGENTS].x, vert.attributes[GDVAT_TANGENTS].y, vert.attributes[GDVAT_TANGENTS].z, vert.attributes[GDVAT_TANGENTS].w );
	fout << buffer;
}

template<>
void WriteAttributeBinary<GDVAT_TANGENTS>(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[GDVAT_TANGENTS].x, sizeof(vert.attributes[GDVAT_TANGENTS].x)*4 );
}

template<>
unsigned int AttributeByteCount<GDVAT_TANGENTS>(void)
{
	return 16;
}