#pragma once


#include "../EDGOCA/ComponentSystem.h"
#include "../tinyxml/tinyxml.h"
#include "../EDUtilities/XMLLoadingMethods.h"
	
template<>
void EDGOCA::Attribute<std::string>::ReadXML(TiXmlElement* valElement)
{
	if( valElement->GetText() != 0 )
		value = valElement->GetText();
}

template<>
void EDGOCA::Attribute<float>::ReadXML(TiXmlElement* valElement)
{
	if( valElement->GetText() != 0 )
		value = (float)atof( valElement->GetText() );
}

template<>
void EDGOCA::Attribute<DirectX::XMFLOAT4>::ReadXML(TiXmlElement* valElement)
{
	TiXmlElement* xEle = valElement->FirstChildElement("x");
	if( xEle != 0 && xEle->GetText() != 0 )
		value.x = (float)atof( xEle->GetText() );

	TiXmlElement* yEle = valElement->FirstChildElement("y");
	if( yEle != 0 && yEle->GetText() != 0 )
		value.y = (float)atof( yEle->GetText() );

	TiXmlElement* zEle = valElement->FirstChildElement("z");
	if( zEle != 0 && zEle->GetText() != 0 )
		value.z = (float)atof( zEle->GetText() );

	TiXmlElement* wEle = valElement->FirstChildElement("w");
	if( wEle != 0 && wEle->GetText() != 0 )
		value.w = (float)atof( wEle->GetText() );
}

template<>
void EDGOCA::Attribute<DirectX::XMFLOAT3>::ReadXML(TiXmlElement* valElement)
{
	TiXmlElement* xEle = valElement->FirstChildElement("x");
	if( xEle != 0 && xEle->GetText() != 0 )
		value.x = (float)atof( xEle->GetText() );

	TiXmlElement* yEle = valElement->FirstChildElement("y");
	if( yEle != 0 && yEle->GetText() != 0 )
		value.y = (float)atof( yEle->GetText() );

	TiXmlElement* zEle = valElement->FirstChildElement("z");
	if( zEle != 0 && zEle->GetText() != 0 )
		value.z = (float)atof( zEle->GetText() );
}

