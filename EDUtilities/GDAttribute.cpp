
#include "GDAttribute.h"
#include "ContentManager.h"
#include "GDMeshStream.h"
#include "GDAttributeTypes.h"

#include "../tinyxml/tinyxml.h"

using namespace EDUtilities;

void GDAttributeBase::Read(const char* fileName)
{
	ContentHandle<GDMeshStreamBinary> meshStreamHandle = ContentManager::Load<GDMeshStreamBinary>(fileName);

	GDMeshStreamBinary* meshStreamPtr = meshStreamHandle.GetContent();

	if( meshStreamPtr != 0 )
	{
		std::ifstream* pAttrTypeListPtr = meshStreamPtr->GetAttributeTypeListPtr( GetAttributeTypeName(), GetAttributeName() );

		if( pAttrTypeListPtr != 0 )
			Read(pAttrTypeListPtr);
	}
}

void GDAttributeBase::ReadXML(const char* meshXMLFileName)
{
	ContentHandle<GDMeshStreamXML> meshStreamHandle = 
		ContentManager::LoadXML<GDMeshStreamXML>(meshXMLFileName);
	
	GDMeshStreamXML* meshStreamPtr = meshStreamHandle.GetContent();

	if( meshStreamPtr != 0 )
	{
		TiXmlElement* pAttrTypeListEle = meshStreamPtr->GetAttributeTypeListElement( GetAttributeTypeName(), GetAttributeName() );

		if( pAttrTypeListEle != 0 )
			ReadXML(pAttrTypeListEle);
	}
		
	//TiXmlDocument doc(meshXMLFileName);

	//if(!doc.LoadFile())
	//	return;

	//TiXmlHandle hDoc(&doc);
	//TiXmlElement* elementPtr;
	//TiXmlHandle hRoot(0);
	//	
	//elementPtr = hDoc.FirstChildElement().Element();
	//if( !elementPtr )
	//	return;

	//std::string rootName = elementPtr->Value();

	//if( strcmp(rootName.c_str(), "GDMesh") != 0 )
	//	return;

	//hRoot = TiXmlHandle(elementPtr);

	//TiXmlElement* childPtr = elementPtr->FirstChildElement("GDMeshAttribute_List");

	//if( childPtr == 0 )
	//	return;

	//childPtr = childPtr->FirstChildElement("GDMeshAttribute");
	//TiXmlElement* listElementPtr = 0;
	//
	//std::string typeList = GetAttributeTypeName();
	//typeList += "_List";

	//for(; childPtr != 0; childPtr = childPtr->NextSiblingElement("GDMeshAttribute"))
	//{
	//	TiXmlElement* pCurrent = childPtr->FirstChildElement(typeList.c_str());
	//	if( pCurrent != 0 )
	//	{
	//		const char* _name = pCurrent->Attribute("name");
	//		if( _name == 0 && GetAttributeName() == 0 )
	//		{
	//			listElementPtr = pCurrent;
	//			break;
	//		}
	//		else if( _name != 0 && GetAttributeName() != 0 )
	//		{
	//			if( strcmp( _name, GetAttributeName() ) == 0 )
	//			{
	//				listElementPtr = pCurrent;
	//				break;
	//			}
	//		}
	//	}
	//}

	//if( listElementPtr != 0 )
	//	ReadXML( listElementPtr );
}

int GDAttributeBase::GetItemCount(TiXmlElement* itemListElementPtr)
{
	const char* countPtr = itemListElementPtr->Attribute("count");

	if( countPtr != 0 )
		return atoi(countPtr);

	int count = 0;

	TiXmlElement* childPtr = itemListElementPtr->FirstChildElement(GetAttributeTypeName());
	
	for(; childPtr != 0; childPtr = childPtr->NextSiblingElement(GetAttributeTypeName()) )
		++count;

	return count;
}

void GDAttributeBase::ReadXML(TiXmlElement* elementPtr)
{
	int count = GetItemCount(elementPtr);

	if( count == 0 )
		return;

	resize( count );

	TiXmlElement* childPtr = elementPtr->FirstChildElement(GetAttributeTypeName());
	
	for(int index = 0; childPtr != 0 && index < count; childPtr = childPtr->NextSiblingElement(GetAttributeTypeName()), ++index )
		ReadXMLItem( childPtr, index );
}

void GDAttributeBase::Read(std::ifstream* fileStreamPtr)
{
	unsigned int count;
	fileStreamPtr->read( (char*)&count, sizeof(count) );

	//if( count == 0 )
	//	return;

	fileStreamPtr->read( (char*)&count, sizeof(count) );
	if( count == 0 )
		return;

	resize( count );
	for(unsigned int index = 0; index < count; ++index)
		ReadItem( fileStreamPtr, index );
}

void GDAttributeBase::ReadTransformXML(TiXmlElement* elementPtr, float* transformPtr)
{
	TiXmlElement* childPtr = elementPtr->FirstChildElement("XAxis");
	
	if( childPtr != 0 )
		ReadVector4XML( childPtr, transformPtr );

	childPtr = elementPtr->FirstChildElement("YAxis");
	
	if( childPtr != 0 )
		ReadVector4XML( childPtr, &transformPtr[4] );

	childPtr = elementPtr->FirstChildElement("ZAxis");
	
	if( childPtr != 0 )
		ReadVector4XML( childPtr, &transformPtr[8] );

	childPtr = elementPtr->FirstChildElement("WAxis");
	
	if( childPtr != 0 )
		ReadVector4XML( childPtr, &transformPtr[12] );
}

void GDAttributeBase::ReadVector3XML(TiXmlElement* elementPtr, float* vectorPtr)
{
	TiXmlElement *childPtr = elementPtr->FirstChildElement("x");
	if( childPtr == 0 )
		return;
	
	vectorPtr[0] = (float)atof(childPtr->GetText());

	childPtr = childPtr->NextSiblingElement("y");//elementPtr->FirstChildElement("y");
	if( childPtr == 0 )
		return;
	
	vectorPtr[1] = (float)atof(childPtr->GetText());

	childPtr = childPtr->NextSiblingElement("z"); //elementPtr->FirstChildElement("z");
	if( childPtr == 0 )
		return;
	
	vectorPtr[2] = (float)atof(childPtr->GetText());
}

void GDAttributeBase::ReadVector4XML(TiXmlElement* elementPtr, float* vectorPtr)
{
	TiXmlElement *childPtr = elementPtr->FirstChildElement("x");
	if( childPtr == 0 )
		return;
	
	vectorPtr[0] = (float)atof(childPtr->GetText());

	childPtr = childPtr->NextSiblingElement("y");//elementPtr->FirstChildElement("y");
	if( childPtr == 0 )
		return;
	
	vectorPtr[1] = (float)atof(childPtr->GetText());

	childPtr = childPtr->NextSiblingElement("z");//elementPtr->FirstChildElement("z");
	if( childPtr == 0 )
		return;
	
	vectorPtr[2] = (float)atof(childPtr->GetText());

	childPtr = childPtr->NextSiblingElement("w");//elementPtr->FirstChildElement("w");
	if( childPtr == 0 )
		return;
	
	vectorPtr[3] = (float)atof(childPtr->GetText());
}

void GDAttributeBase::ReadColor4XML(TiXmlElement* elementPtr, float* vectorPtr)
{
	TiXmlElement *childPtr = elementPtr->FirstChildElement("r");
	if( childPtr == 0 )
		return;
	
	vectorPtr[0] = (float)atof(childPtr->GetText());

	childPtr = elementPtr->FirstChildElement("g");
	if( childPtr == 0 )
		return;
	
	vectorPtr[1] = (float)atof(childPtr->GetText());

	childPtr = elementPtr->FirstChildElement("b");
	if( childPtr == 0 )
		return;
	
	vectorPtr[2] = (float)atof(childPtr->GetText());

	childPtr = elementPtr->FirstChildElement("a");
	if( childPtr == 0 )
		return;
	
	vectorPtr[3] = (float)atof(childPtr->GetText());
}

void GDAttributeBase::ReadAabbXML(TiXmlElement* elementPtr, EDMath::Aabb* aabbPtr)
{
	TiXmlElement *childPtr = elementPtr->FirstChildElement("Min");
	if( childPtr == 0 )
		return;
	
	//childPtr = childPtr->FirstChildElement("x");
	//
	//if( childPtr == 0 )
	//	return;

	//aabbPtr->min.x = (float)atof( childPtr->GetText() );
	//
	//childPtr = childPtr->NextSiblingElement("y");
	//
	//if( childPtr == 0 )
	//	return;

	//aabbPtr->min.y = (float)atof( childPtr->GetText() );

	//childPtr = childPtr->NextSiblingElement("z");
	//
	//if( childPtr == 0 )
	//	return;

	//aabbPtr->min.z = (float)atof( childPtr->GetText() );

	//childPtr = elementPtr->FirstChildElement("Max");
	//if( childPtr == 0 )
	//	return;
	//
	//childPtr = childPtr->FirstChildElement("x");
	//
	//if( childPtr == 0 )
	//	return;

	//aabbPtr->max.x = (float)atof( childPtr->GetText() );
	//
	//childPtr = childPtr->NextSiblingElement("y");
	//
	//if( childPtr == 0 )
	//	return;

	//aabbPtr->max.y = (float)atof( childPtr->GetText() );

	//childPtr = childPtr->NextSiblingElement("z");
	//
	//if( childPtr == 0 )
	//	return;

	//aabbPtr->max.z = (float)atof( childPtr->GetText() );
}

void GDAttributeBase::ReadSphereXML(TiXmlElement* elementPtr, EDMath::Sphere* spherePtr)
{
	TiXmlElement *childPtr = elementPtr->FirstChildElement("Center");
	if( childPtr == 0 )
		return;
	
	childPtr = childPtr->FirstChildElement("x");
	
	if( childPtr == 0 )
		return;

	spherePtr->center.x = (float)atof( childPtr->GetText() );
	
	childPtr = childPtr->NextSiblingElement("y");
	
	if( childPtr == 0 )
		return;

	spherePtr->center.y = (float)atof( childPtr->GetText() );

	childPtr = childPtr->NextSiblingElement("z");
	
	if( childPtr == 0 )
		return;

	spherePtr->center.z = (float)atof( childPtr->GetText() );

	childPtr = elementPtr->FirstChildElement("Radius");

	if( childPtr == 0 )
		return;

	spherePtr->radius = (float)atof(childPtr->GetText());
}
