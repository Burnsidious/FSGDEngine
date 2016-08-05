#include "DirectionalLightSource.h"

#include "../EDRendererD3D/RenderShape.h"

#include "RenderController.h"

using namespace EDRendererD3D;

DirectionalLightSource::DirectionalLightSource(void)
{
	lightPtr = 0;
}

DirectionalLightSource::DirectionalLightSource(const DirectionalLightSource &rhs) 
	: EDGameCore::DirectionalLight<DirectionalLightSource>(rhs)
{
	lightPtr = 0;
	shadows = rhs.shadows;
	CreateLightPointer();
	*lightPtr = *rhs.lightPtr;
}

DirectionalLightSource::~DirectionalLightSource()
{
	delete lightPtr;
}

void DirectionalLightSource::Render(void)
{
	lightPtr->SetWorldMatrixPtr((const XMFLOAT4X4 *)&GetTransform()->GetWorldMatrix());
}

void DirectionalLightSource::LoadState(TiXmlElement* xmlElement)
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
	DirectX::XMFLOAT3 direction(0.0f, -1.0f, 0.0f);
	float specularPower;
	float specularIntensity;

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

	TiXmlElement* directionElement = xmlElement->FirstChildElement("Direction");
	if( directionElement != 0 )
	{
		
		TiXmlElement* component = directionElement->FirstChildElement("x");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			direction.x = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = directionElement->FirstChildElement("y");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			direction.y = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );

		component = directionElement->FirstChildElement("z");
		
		if( component != 0 &&
			component->GetText() != 0 &&
			strlen(component->GetText()) != 0 )
			direction.z = max( 0.0f, min( 1.0f, (float)atof(component->GetText()) ) );
	}

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


	((DirectionalLightWithShadow *)lightPtr)->Initialize(
		"GDForm/DirectionalLight/frm_DirectionalLightDeferred.xml",
		RenderController::GetInstance()->GetDirViewProjection(),
		&diffuseColor, &ambientColor, &specularColor, specularPower,
		specularIntensity, RenderController::theDirectionalShadowSize,
		RenderController::theDirectionalShadowSize);

}

void DirectionalLightSource::CreateLightPointer()
{
	delete lightPtr;
	lightPtr = new DirectionalLightWithShadow();
}