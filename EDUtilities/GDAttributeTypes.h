#pragma once

#include "GDAttribute.h"
//#include "../EDTree/EDTreeNode.h"
//#include "../EDTree/AabbBvhInternalNode.h"
//#include "../EDTree/AabbBvhLeafNode.h"

// POINT ATTRIBUTE
class Point : public EDMath::Float3 {};

template<>
void GDAttribute<Point>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadVector3XML( itemElementPtr, values[itemIndex].v );
}

template<>
void GDAttribute<Point>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].v, sizeof(Point) );
}

template<>
const char* GDAttribute<Point>::_getAttributeTypeName(void){ return "Point";}
///////////////////

// UV ATTRIBUTE
union UV
{
	struct
	{
		float u;
		float v;
	};

	float uv[2];
};

template<>
void GDAttribute<UV>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].uv, sizeof(UV) );
}

template<>
void GDAttribute<UV>::ReadXMLItem(TiXmlElement *itemElementPtr, unsigned int itemIndex)
{
	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("s");
	if( childPtr == 0 )
		return;

	values[itemIndex].u = (float)atof( childPtr->GetText() );

	childPtr = itemElementPtr->FirstChildElement("t");
	if( childPtr == 0 )
		return;

	values[itemIndex].v = (float)atof( childPtr->GetText() );
}

template<>
const char* GDAttribute<UV>::_getAttributeTypeName(void){ return "TexCoord";}
///////////////////

// NORMAL ATTRIBUTE
class Normal : public EDMath::Float3 {};

template<>
void GDAttribute<Normal>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadVector3XML( itemElementPtr, values[itemIndex].v );
}

template<>
void GDAttribute<Normal>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].v, sizeof(Normal) );
}

template<>
const char* GDAttribute<Normal>::_getAttributeTypeName(void){ return "Normal";}
///////////////////


// TANGENT ATTRIBUTE
class Tangent : public EDMath::Float3 {};

template<>
void GDAttribute<Tangent>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadVector3XML( itemElementPtr, values[itemIndex].v );
}

template<>
void GDAttribute<Tangent>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	float buff[4];
	fileStreamPtr->read( (char*)buff, sizeof(float)*4 );
	memcpy_s( values[itemIndex].v, sizeof(float)*3, buff, sizeof(float)*3 );
}

template<>
const char* GDAttribute<Tangent>::_getAttributeTypeName(void){ return "Tangent";}
///////////////////


// BITANGENT ATTRIBUTE
class Bitangent : public EDMath::Float3 {};

template<>
void GDAttribute<Bitangent>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadVector3XML( itemElementPtr, values[itemIndex].v );
}

template<>
void GDAttribute<Bitangent>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].v, sizeof(Bitangent) );
}

template<>
const char* GDAttribute<Bitangent>::_getAttributeTypeName(void){ return "Bitangent";}
///////////////////

//// COLOR ATTRIBUTE
union Color
{
	struct
	{
		float Red;
		float Green;
		float Blue;
		float Alpha;
	};

	float RGBA[4];
};

template<>
void GDAttribute<Color>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadColor4XML( itemElementPtr, values[itemIndex].RGBA );
}

template<>
void GDAttribute<Color>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].RGBA, sizeof(Color) );
}

template<>
const char* GDAttribute<Color>::_getAttributeTypeName(void){ return "Color";}
/////////////////////

// TRINORMAL ATTRIBUTE
class TriNormal : public EDMath::Float3 {};

template<>
void GDAttribute<TriNormal>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadVector3XML( itemElementPtr, values[itemIndex].v );
}

template<>
void GDAttribute<TriNormal>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].v, sizeof(TriNormal) );
}

template<>
const char* GDAttribute<TriNormal>::_getAttributeTypeName(void){ return "TriNormal";}
///////////////////

// INFLUENCE ATTRIBUTE
//struct Influence
//{
//	int jointIndices[4];
//	float weights[4];
//};
//
//template<>
//void GDAttribute<Influence>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
//{
//	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("JointIndex");
//		
//	for(unsigned int i = 0; i < 4 && childPtr != 0; ++i)
//	{
//		values[itemIndex].jointIndices[i] = atoi( childPtr->GetText() );
//			
//		TiXmlElement* pSibling = childPtr->NextSiblingElement("Weight");
//		if( pSibling != 0 )
//			values[itemIndex].weights[i] = (float)atof( pSibling->GetText() );
//			
//		childPtr = childPtr->NextSiblingElement("JointIndex");
//	}
//}
//
//template<>
//const char* GDAttribute<Influence>::_getAttributeTypeName(void){ return "Influence";}

union JointIndices
{
	int jointIndices[4];
	
	struct
	{
		int joint0;
		int joint1;
		int joint2;
		int joint3;
	};
};

template<>
void GDAttribute<JointIndices>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("JointIndex");
	
	for(unsigned int i = 0; i < 4 && childPtr != 0; ++i)
	{
		values[itemIndex].jointIndices[i] = atoi( childPtr->GetText() );
		childPtr = childPtr->NextSiblingElement("JointIndex");
	}
}

template<>
void GDAttribute<JointIndices>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].jointIndices, sizeof(JointIndices) );
}

template<>
const char* GDAttribute<JointIndices>::_getAttributeTypeName(void){ return "JointIndices"; }

union Weights
{
	float weights[4];
	
	struct
	{
		float weight0;
		float weight1;
		float weight2;
		float weight3;
	};
};

template<>
const char* GDAttribute<Weights>::_getAttributeTypeName(void){ return "Weights"; }

template<>
void GDAttribute<Weights>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("WeightValue");
	for(unsigned int i = 0; i < 4 && childPtr != 0; ++i)
	{
		values[itemIndex].weights[i] = (float)atof( childPtr->GetText() );
		childPtr = childPtr->NextSiblingElement("WeightValue");
	}
}

template<>
void GDAttribute<Weights>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].weights, sizeof(Weights) );
}


///////////////////

// JOINT ATTRIBUTE
struct Joint
{
	EDMath::Float4x4 localTransform;
	char name[16];
	char parentName[16];
};

template<>
void GDAttribute<Joint>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("Name");
	if( childPtr == 0 )
		return;

	strcpy_s( values[itemIndex].name, childPtr->GetText() );

	childPtr = itemElementPtr->FirstChildElement("ParentName");

	if( childPtr == 0 )
		return;

	const char* parentName = childPtr->GetText();

	if( parentName != 0 )
		strcpy_s( values[itemIndex].parentName, parentName );
	else
		memset( values[itemIndex].parentName, 0, sizeof(values[itemIndex].parentName) ); 

	childPtr = itemElementPtr->FirstChildElement("LocalTransform");

	if( childPtr == 0 )
		return;

	ReadTransformXML( childPtr, values[itemIndex].localTransform.ma );
}

template<>
void GDAttribute<Joint>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].localTransform.ma, sizeof(Joint) );
}

template<>
const char* GDAttribute<Joint>::_getAttributeTypeName(void){ return "Joint";}
///////////////////


// SPHERE ATTRIBUTE
template<>
void GDAttribute<EDMath::Sphere>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadSphereXML( itemElementPtr, &values[itemIndex] );
}

template<>
void GDAttribute<EDMath::Sphere>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].center.v, sizeof(EDMath::Float3) );
	fileStreamPtr->read( (char*)&values[itemIndex].radius, sizeof(float) );
}

template<>
const char* GDAttribute<EDMath::Sphere>::_getAttributeTypeName(void){ return "Sphere"; }
///////////////////


// AABB ATTRIBUTE
template<>
void GDAttribute<EDMath::Aabb>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	ReadAabbXML( itemElementPtr, &values[itemIndex] );
}

template<>
void GDAttribute<EDMath::Aabb>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	EDMath::Float3 vmin;
	EDMath::Float3 vmax;
	fileStreamPtr->read( (char*)vmin.v, sizeof(EDMath::Float3) );
	fileStreamPtr->read( (char*)vmax.v, sizeof(EDMath::Float3) );

	values[itemIndex].center = (vmax + vmin) * 0.5f;
	values[itemIndex].extents = vmax - values[itemIndex].center;
}

template<>
const char* GDAttribute<EDMath::Aabb>::_getAttributeTypeName(void){ return "AABB"; }
///////////////////

union TriVertIndices
{
	struct
	{
		unsigned int index0;
		unsigned int index1;
		unsigned int index2;
	};

	unsigned int indices[3];
};

template<>
void GDAttribute<TriVertIndices>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("Index");

	for(unsigned int i = 0; i < 3 && childPtr != 0; ++i, childPtr = childPtr->NextSiblingElement())
		values[itemIndex].indices[i] = atoi( childPtr->GetText() );
}

template<>
void GDAttribute<TriVertIndices>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	fileStreamPtr->read( (char*)values[itemIndex].indices, sizeof(TriVertIndices) );
}

template<>
const char* GDAttribute<TriVertIndices>::_getAttributeTypeName(void){ return "Triangle"; }
///////////////////

struct BVHNode
{
	EDMath::Aabb AABB;
	bool isLeaf;
	std::vector< unsigned int> indices;
};

template<>
void GDAttribute<BVHNode>::ReadXMLItem(TiXmlElement* itemElementPtr, unsigned int itemIndex)
{
	TiXmlElement* childPtr = itemElementPtr->FirstChildElement("AABB");
	ReadAabbXML( childPtr, &values[itemIndex].AABB );
	
	childPtr = itemElementPtr->FirstChildElement("Triangle_List");

	if( childPtr != 0 )
	{
		values[itemIndex].isLeaf = true;
		
		TiXmlElement* pTri = childPtr->FirstChildElement("Triangle");

		for(; pTri != 0; pTri = pTri->NextSiblingElement("Triangle") )
		{
			TiXmlElement* pTriIndex = pTri->FirstChildElement("TriIndex");
			values[itemIndex].indices.push_back( atoi( pTriIndex->GetText() ) );
		}
	}
	else
	{
		childPtr = itemElementPtr->FirstChildElement("ChildIndex_List");

		values[itemIndex].isLeaf = false;
		
		TiXmlElement* pIndex = childPtr->FirstChildElement("ChildIndex");

		for(; pIndex != 0 ; pIndex = pIndex->NextSiblingElement("ChildIndex"))
			values[itemIndex].indices.push_back( atoi( pIndex->GetText() ) );
	}
}

template<>
void GDAttribute<BVHNode>::ReadItem(std::ifstream* fileStreamPtr, unsigned int itemIndex)
{
	EDMath::Float3 vmin;
	EDMath::Float3 vmax;
	fileStreamPtr->read( (char*)vmin.v, sizeof(EDMath::Float3) );
	fileStreamPtr->read( (char*)vmax.v, sizeof(EDMath::Float3) );

	values[itemIndex].AABB.center = (vmax + vmin) * 0.5f;
	values[itemIndex].AABB.extents = vmax - values[itemIndex].AABB.center;

	//fileStreamPtr->read( (char*)values[itemIndex].AABB.min.v, sizeof(EDMath::Float3) );
	//fileStreamPtr->read( (char*)values[itemIndex].AABB.max.v, sizeof(EDMath::Float3) );
	unsigned int leafFlag;
	fileStreamPtr->read( (char*)&leafFlag, sizeof(leafFlag) );
	values[itemIndex].isLeaf = (leafFlag == 1 );
	
	fileStreamPtr->read( (char*)&leafFlag, sizeof(leafFlag) );
	values[itemIndex].indices.resize( leafFlag );
	fileStreamPtr->read( (char*)&values[itemIndex].indices[0], sizeof(unsigned int)*leafFlag );
}

template<>
const char* GDAttribute<BVHNode>::_getAttributeTypeName(void){ return "BVHNode"; }



