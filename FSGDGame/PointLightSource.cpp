#include "PointLightSource.h"

#include "../EDRendererD3D/RenderShape.h"

#include "RenderController.h"

using namespace EDRendererD3D;
using namespace DirectX;

PointLightSource::PointLightSource(void)
{
	lightPtr = 0;
}

PointLightSource::PointLightSource(const PointLightSource& rhs) : EDGameCore::PointLight<PointLightSource>(rhs)
{
	lightPtr = 0;
	shadows = rhs.shadows;
	CreateLightPointer();
	if(LightShadows::SOFT == shadows)
		*((PointLightWithShadow *)lightPtr) = *((PointLightWithShadow *)rhs.lightPtr);
	else
		*lightPtr = *rhs.lightPtr;
}

PointLightSource::~PointLightSource()
{
	delete lightPtr;
}

void PointLightSource::Render(void)
{
	Float3 pos = GetTransform()->GetWorldMatrix().WAxis;
	XMFLOAT4X4 newMat = *lightPtr->GetWorldMatrixPtr();
	newMat._41 = pos.x;
	newMat._42 = pos.y;
	newMat._43 = pos.z;
	newMat._11 = newMat._22 = newMat._33 = lightPtr->GetRange();
	lightPtr->SetWorldMatrixPtr(&newMat);

	if(LightShadows::SOFT == shadows)
		RenderController::GetInstance()->AddVisiblePointLightShadowed(this);
	else
		RenderController::GetInstance()->AddVisiblePointLight(this);
}


void PointLightSource::LoadState(TiXmlElement* xmlElement)
{
	TiXmlElement *shadow = xmlElement->FirstChildElement("CastShadows");
	if(shadow)
	{
		const char *shadowStateText = shadow->GetText();
		// Checking for false which could be represented as 0, "no", "No", "false" or "FaLse"
		if(shadowStateText[0] == '0' || toupper(shadowStateText[0]) == 'N' 
			|| toupper(shadowStateText[0]) == 'F')
			shadows = LightShadows::NONE;
		else
			shadows = LightShadows::SOFT;
	}
	else
	{
		shadows = LightShadows::NONE;
	}

	DirectX::XMFLOAT3 diffuseColor(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 ambientColor(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 specularColor(0.0f, 0.0f, 0.0f);
	float specularPower;
	float specularIntensity;
	DirectX::XMFLOAT3 attenuationValue(1.0f, 0.0f, 0.0f);;
	float radiusValue = 1.0f;

	if( lightPtr == 0 )
	{
		CreateLightPointer();
	}

	TiXmlElement* diffuse = xmlElement->FirstChildElement("Diffuse");
	if( diffuse != 0 )
	{
		
		TiXmlElement* component = diffuse->FirstChildElement("r");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			diffuseColor.x = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = diffuse->FirstChildElement("g");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			diffuseColor.y = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = diffuse->FirstChildElement("b");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			diffuseColor.z = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );
	}

	TiXmlElement* specular = xmlElement->FirstChildElement("Specular");
	if( specular != 0 )
	{
		
		TiXmlElement* component = specular->FirstChildElement("r");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			specularColor.x = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = specular->FirstChildElement("g");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			specularColor.y = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = specular->FirstChildElement("b");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			specularColor.z = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

	}

	TiXmlElement* ambient = xmlElement->FirstChildElement("Ambient");
	if( ambient != 0 )
	{
		
		TiXmlElement* component = ambient->FirstChildElement("r");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			ambientColor.x = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = ambient->FirstChildElement("g");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			ambientColor.y = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = ambient->FirstChildElement("b");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			ambientColor.z = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

	}

	TiXmlElement* attenuation = xmlElement->FirstChildElement("Attenuation");
	if( attenuation != 0)
	{
		TiXmlElement* component = attenuation->FirstChildElement("Constant");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			attenuationValue.x = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = attenuation->FirstChildElement("Linear");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			attenuationValue.y = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = attenuation->FirstChildElement("Quadratic");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			attenuationValue.z = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );
	}

	TiXmlElement* radiusEle = xmlElement->FirstChildElement("Range");

	if( radiusEle != 0 &&
		radiusEle->GetText() != 0 &&
		strlen( radiusEle->GetText() ) != 0 )
		radiusValue = (float)atof( radiusEle->GetText() );


	TiXmlElement* specEle = xmlElement->FirstChildElement("SpecularIntensity");
	if( specEle != 0 &&
		specEle->GetText() != 0 &&
		strlen( specEle->GetText() ) != 0 )
		specularIntensity = (float)atof( specEle->GetText() );
	else
		specularIntensity = 1.0f;

	specEle = xmlElement->FirstChildElement("SpecularPower");
	if( specEle != 0 &&
		specEle->GetText() != 0 &&
		strlen( specEle->GetText() ) != 0 )
		specularPower = (float)atof( specEle->GetText() );
	else
		specularPower = 66.0f;

	if(LightShadows::SOFT == shadows)
	{
		((PointLightWithShadow *)lightPtr)->Initialize(
			"GDForm/PointLight/frm_PointLightWithShadowsDeferred.xml",
			&diffuseColor, &ambientColor, &specularColor, &attenuationValue, specularPower, 
			specularIntensity, radiusValue, RenderController::GetPointShadowSize());
	}
	else
	{
		lightPtr->Initialize(
			"GDForm/PointLight/frm_PointLightDeferred.xml", &diffuseColor, 
			&ambientColor, &specularColor, specularPower, specularIntensity, &attenuationValue, 
			radiusValue);
	}

	SetRange(radiusValue);
	SetColor(Float3(diffuseColor.x + ambientColor.x, diffuseColor.y + ambientColor.y, diffuseColor.z + ambientColor.z));
}

void PointLightSource::CreateLightPointer()
{
	delete lightPtr;
	if(LightShadows::SOFT == shadows)
	{
		lightPtr = new PointLightWithShadow();
	}
	else
	{
		lightPtr = new EDRendererD3D::PointLight();
	}
}

