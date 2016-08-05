#include "GDAttribute.h"

const char* GDVertexAttribTypeNames[] = { "Point", "Normal", "Tangent", "Bitangent", "TexCoord", "TexCoord", "TexCoord", "JointIndices", "Weights" };
const char* GDTriangleAttribTypeNames[] = { "TriNormal", "TriNeighbor" };
const char* GDMeshAttributeTypeNames[] = { "Sphere", "AABB", "BVH" };

void WriteVector4fXML(std::ofstream& fout, const float *pVector)
{
	char buffer[128];
	sprintf_s( buffer, "<x>%f</x><y>%f</y><z>%f</z><w>%f</w>\n", pVector[0], pVector[1], pVector[2], pVector[3] );
	fout << buffer;
}

void WriteTransformXML(std::ofstream& fout, const float transform[4][4])
{
	fout << "<XAxis>";
	WriteVector4fXML( fout, transform[0] );
	fout << "</XAxis>\n";

	fout << "<YAxis>";
	WriteVector4fXML( fout, transform[1] );
	fout << "</YAxis>\n";

	fout << "<ZAxis>";
	WriteVector4fXML( fout, transform[2] );
	fout << "</ZAxis>\n";

	fout << "<WAxis>";
	WriteVector4fXML( fout, transform[3] );
	fout << "</WAxis>\n";
}

//void WriteAabbXML(std::ofstream& fout, const EDCollision::Aabb* aabbPtr)
//{
//	const char* aabbstring = "<Min><x>%f</x><y>%f</y><z>%f</z></Min><Max><x>%f</x><y>%f</y><z>%f</z></Max>\n";
//	char buffer[256];
//	
//	sprintf_s( buffer, aabbstring, aabbPtr->min.x, aabbPtr->min.y, aabbPtr->min.z, aabbPtr->max.x, aabbPtr->max.y, aabbPtr->max.z ); 
//	fout << buffer;
//}