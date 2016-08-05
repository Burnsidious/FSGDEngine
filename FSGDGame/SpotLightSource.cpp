#include "SpotLightSource.h"

#include "../EDRendererD3D/RenderShape.h"

#include "RenderController.h"

using namespace EDRendererD3D;
using namespace DirectX;

SpotLightSource::SpotLightSource(void)
{
	lightPtr = 0;	
}

SpotLightSource::SpotLightSource(const SpotLightSource& rhs) : EDGameCore::SpotLight< SpotLightSource >(rhs)
{
	lightPtr = 0;	
	shadows = rhs.shadows;
	CreateLightPointer();
	
	if(LightShadows::SOFT == shadows)
		*((SpotLightWithShadow *)lightPtr) = *((SpotLightWithShadow *)rhs.lightPtr);
	else
		*lightPtr = *rhs.lightPtr;
}

SpotLightSource::~SpotLightSource()
{
	delete lightPtr;
}

void SpotLightSource::Render(void)
{
	XMFLOAT3 attenuation;
	XMStoreFloat3(&attenuation, lightPtr->GetAttenuation());

	float adjacent = (-attenuation.y + sqrt( attenuation.y * attenuation.y 
		- 4 * attenuation.z * (attenuation.x - 255.f))) / (2 * attenuation.z);

	float radAngle = acos(lightPtr->GetCutOff());
	float tanr = tan(radAngle);
	float opposite = tanr * adjacent;

	Float4x4 newMat(GetTransform()->GetWorldMatrix());
	newMat.XAxis *= opposite;
	newMat.YAxis *= opposite;
	newMat.ZAxis *= adjacent;

	lightPtr->SetWorldMatrixPtr((XMFLOAT4X4 *)&newMat);

	if(LightShadows::SOFT == shadows)
		RenderController::GetInstance()->AddVisibleLight(((SpotLightWithShadow *)lightPtr));
	else
		RenderController::GetInstance()->AddVisibleLight(lightPtr);
}


void SpotLightSource::LoadState(TiXmlElement* xmlElement)
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

	if( lightPtr == 0 )
	{
		CreateLightPointer();
	}

	TiXmlElement* diffuse = xmlElement->FirstChildElement("Diffuse");
	DirectX::XMFLOAT3 diffuseColor(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 ambientColor(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 specularColor(0.0f, 0.0f, 0.0f);
	float specularPower;
	float specularIntensity;
	float cutOffValue;
	float exponentValue;
	DirectX::XMFLOAT3 attenuationValue(1.0f, 0.0125f, 0.0125f);
	float radiusValue = 25.0f;

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
		specularIntensity = 3.0f;

	specEle = xmlElement->FirstChildElement("SpecularPower");
	if( specEle != 0 &&
		specEle->GetText() != 0 &&
		strlen( specEle->GetText() ) != 0 )
		specularPower = (float)atof( specEle->GetText() );
	else
		specularPower = 36.0f;

	TiXmlElement* cutOffEle = xmlElement->FirstChildElement("CutOff");
	if( cutOffEle != 0 &&
		cutOffEle->GetText() != 0 &&
		strlen( cutOffEle->GetText() ) != 0 )
		cutOffValue = (float)atof(cutOffEle->GetText());
	else
		cutOffValue = .75f;
	
	TiXmlElement* exponentEle = xmlElement->FirstChildElement("Exponent");
	if( exponentEle != 0 &&
		exponentEle->GetText() != 0 &&
		strlen( exponentEle->GetText() ) != 0 )
		exponentValue = (float)atof(exponentEle->GetText());
	else
		exponentValue = 1.0f;

	if(LightShadows::SOFT == shadows)
	{
		((SpotLightWithShadow *)lightPtr)->Initialize(
			"GDForm/SpotLight/frm_SpotLightWithShadowsDeferred.xml",
			&diffuseColor, &ambientColor, &specularColor, &attenuationValue, specularPower, 
			specularIntensity, cutOffValue, exponentValue, radiusValue, 
			&RenderController::GetInstance()->GetSpotShadowRenderTarget());
	}
	else
	{
		lightPtr->Initialize(
			"GDForm/SpotLight/frm_SpotLightDeferred.xml", &diffuseColor, 
			&ambientColor, &specularColor, specularPower, specularIntensity, cutOffValue, 
			exponentValue, &attenuationValue, radiusValue);
	}

	SetRange(radiusValue);
	SetSpotAngle( cutOffValue );
}

void SpotLightSource::CreateLightPointer()
{
	delete lightPtr;
	if(LightShadows::SOFT == shadows)
	{
		lightPtr = new SpotLightWithShadow();
	}
	else
	{
		lightPtr = new EDRendererD3D::SpotLight();
	}
}
