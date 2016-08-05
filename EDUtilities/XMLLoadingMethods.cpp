
#include "XMLLoadingMethods.h"

using namespace EDMath;

void ReadXmlVector3( TiXmlElement* axisElementPtr, float* vectorPtr )
{
	TiXmlElement* pComponent = axisElementPtr->FirstChildElement("x");
	if( pComponent == 0 )
		vectorPtr[0] = 0.0f;
	else if( pComponent->GetText() == 0 )
		vectorPtr[0] = 0.0f;
	else
		vectorPtr[0] = (float)atof( pComponent->GetText() );

	pComponent = axisElementPtr->FirstChildElement("y");
	if( pComponent == 0 )
		vectorPtr[1] = 0.0f;
	else if( pComponent->GetText() == 0 )
		vectorPtr[1] = 0.0f;
	else
		vectorPtr[1] = (float)atof( pComponent->GetText() );

	pComponent = axisElementPtr->FirstChildElement("z");
	if( pComponent == 0 )
		vectorPtr[2] = 0.0f;
	else if( pComponent->GetText() == 0 )
		vectorPtr[2] = 0.0f;
	else
		vectorPtr[2] = (float)atof( pComponent->GetText() );
}

void ReadXmlVector4( TiXmlElement* axisElementPtr, float* vectorPtr )
{
	ReadXmlVector3( axisElementPtr, vectorPtr );

	TiXmlElement* pComponent = axisElementPtr->FirstChildElement("w");
	if( pComponent == 0 )
		vectorPtr[3] = 0.0f;
	else if( pComponent->GetText() == 0 )
		vectorPtr[3] = 0.0f;
	else
		vectorPtr[3] = (float)atof( pComponent->GetText() );
}

void ReadXmlMatrix( TiXmlElement* matrixElementPtr, Float4x4 *matrixPtr );void ReadXmlMatrix( TiXmlElement* matrixElementPtr, Float4x4 *matrixPtr )
{
	TiXmlElement* axisElementPtr = matrixElementPtr->FirstChildElement("XAxis");
	
	if( axisElementPtr != 0 )
		ReadXmlVector4( axisElementPtr, matrixPtr->m[0] );

	axisElementPtr = matrixElementPtr->FirstChildElement("YAxis");
	
	if( axisElementPtr != 0 )
		ReadXmlVector4( axisElementPtr, matrixPtr->m[1] );

	axisElementPtr = matrixElementPtr->FirstChildElement("ZAxis");
	
	if( axisElementPtr != 0 )
		ReadXmlVector4( axisElementPtr, matrixPtr->m[2] );

	axisElementPtr = matrixElementPtr->FirstChildElement("WAxis");
	
	if( axisElementPtr != 0 )
		ReadXmlVector4( axisElementPtr, matrixPtr->m[3] );
}

void ReadXmlString( TiXmlElement* stringElementPtr, std::string& str )
{
	const char* text = stringElementPtr->GetText();
	if( text != 0 &&
		strlen( text ) != 0 )
	{
		str = text;
	}
	else
	{
		str = "";
	}
}
