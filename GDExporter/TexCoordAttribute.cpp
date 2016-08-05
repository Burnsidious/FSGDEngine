#include "TexCoordAttribute.h"

template<>
int AttributeIndex<GDVAT_TEXCOORDS_0>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName)
{
	MStatus result;
	int index;

	result = polyIt.getUVIndex(polyVertNum, index, &MString(pAttributeName));

	if( result == MS::kFailure )
		return -1;

	return index;	
}

template<>
int AttributeIndex<GDVAT_TEXCOORDS_1>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName)
{
	return AttributeIndex<GDVAT_TEXCOORDS_0>(polyIt, polyVertNum, pAttributeName);
}

template<>
int AttributeIndex<GDVAT_TEXCOORDS_2>(MItMeshPolygon &polyIt, unsigned int polyVertNum, const char* pAttributeName)
{
	return AttributeIndex<GDVAT_TEXCOORDS_0>(polyIt, polyVertNum, pAttributeName);
}

template<>
void CollectAttribute<GDVAT_TEXCOORDS_0>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	CollectTexCoordAttribute( mfnMesh, pAttributeSets, 0 );
}

template<>
void CollectAttribute<GDVAT_TEXCOORDS_1>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	CollectTexCoordAttribute( mfnMesh, pAttributeSets, 1 );
}

template<>
void CollectAttribute<GDVAT_TEXCOORDS_2>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	CollectTexCoordAttribute( mfnMesh, pAttributeSets, 2 );
}

template<>
void WriteAttributeXML<GDVAT_TEXCOORDS_0>(std::ofstream &fout, GDVertex &vert)
{
	char buffer[128];
	sprintf_s( buffer, "<s>%f</s><t>%f</t>\n", vert.attributes[GDVAT_TEXCOORDS_0].s, vert.attributes[GDVAT_TEXCOORDS_0].t );
	fout << buffer;
}

template<>
void WriteAttributeXML<GDVAT_TEXCOORDS_1>(std::ofstream &fout, GDVertex &vert)
{
	char buffer[128];
	sprintf_s( buffer, "<s>%f</s><t>%f</t>\n", vert.attributes[GDVAT_TEXCOORDS_1].s, vert.attributes[GDVAT_TEXCOORDS_1].t );
	fout << buffer;
}

template<>
void WriteAttributeXML<GDVAT_TEXCOORDS_2>(std::ofstream &fout, GDVertex &vert)
{
	char buffer[128];
	sprintf_s( buffer, "<s>%f</s><t>%f</t>\n", vert.attributes[GDVAT_TEXCOORDS_2].s, vert.attributes[GDVAT_TEXCOORDS_2].t );
	fout << buffer;
}

template<>
void WriteAttributeBinary<GDVAT_TEXCOORDS_0>(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[GDVAT_TEXCOORDS_0].s, sizeof(vert.attributes[GDVAT_TEXCOORDS_0].s)*2 );
}

template<>
void WriteAttributeBinary<GDVAT_TEXCOORDS_1>(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[GDVAT_TEXCOORDS_1].s, sizeof(vert.attributes[GDVAT_TEXCOORDS_1].s)*2 );
}

template<>
void WriteAttributeBinary<GDVAT_TEXCOORDS_2>(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[GDVAT_TEXCOORDS_2].s, sizeof(vert.attributes[GDVAT_TEXCOORDS_2].s)*2 );
}

template<>
unsigned int AttributeByteCount<GDVAT_TEXCOORDS_0>(void)
{
	return 8;
}

template<>
unsigned int AttributeByteCount<GDVAT_TEXCOORDS_1>(void)
{
	return 8;
}

template<>
unsigned int AttributeByteCount<GDVAT_TEXCOORDS_2>(void)
{
	return 8;
}