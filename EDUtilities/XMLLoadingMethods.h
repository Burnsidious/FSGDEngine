#pragma once
#include "../tinyxml/tinyxml.h"
#include "../EDMath/Float4x4.h"

void ReadXmlVector3( TiXmlElement* axisElementPtr, float* vectorPtr );

void ReadXmlVector4( TiXmlElement* axisElementPtr, float* vectorPtr );

void ReadXmlMatrix( TiXmlElement* matrixElementPtr, EDMath::Float4x4 *matrixPtr );

void ReadXmlString( TiXmlElement* stringElementPtr, std::string& str );
