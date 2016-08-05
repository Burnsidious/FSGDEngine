#include "SkyBox.h"
#include "RenderController.h"

SkyBox::SkyBox(void)
{
	renderShape = 0;
}

SkyBox::SkyBox(const SkyBox& rhs)
{
	renderShape = 0;
	formHandle = rhs.formHandle;
	std::vector< EDUtilities::ContentHandle<EDRendererD3D::RenderForm> > formVector;

	if( formHandle.GetContent() != 0 )
	{
		formVector.push_back(formHandle);

		renderShape = (EDRendererD3D::RenderShape*)formHandle.GetContent()->GetNewRenderShape();
		renderShape->Initialize(formVector);

		RenderController::GetInstance()->AddRenderContext( renderShape->GetRenderContext() );
	}
}

void SkyBox::SetShape(const char* formPath)
{
	if( renderShape != 0 )
	{
		delete renderShape;
		renderShape = 0;
	}

	std::vector< EDUtilities::ContentHandle<EDRendererD3D::RenderForm> > formVector;
	
	formHandle = EDUtilities::ContentManager::LoadXML<EDRendererD3D::RenderForm>( formPath );

	if( formHandle.GetContent() != 0 )
	{
		formVector.push_back(formHandle);

		renderShape = (EDRendererD3D::RenderShape*)formHandle.GetContent()->GetNewRenderShape();
		renderShape->Initialize(formVector);

		RenderController::GetInstance()->AddRenderContext( renderShape->GetRenderContext() );
	}
}

SkyBox::~SkyBox()
{
	delete renderShape;
}

void SkyBox::Render(void)
{
	EDMath::Float4x4 identity;
	identity.makeIdentity();

	renderShape->SetWorldMatrix( (DirectX::XMFLOAT4X4*)&identity );
	renderShape->AddToContextSet();
}

void SkyBox::LoadState(TiXmlElement* xmlElement)
{
	assert(xmlElement !=  0);

	TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

	if( pathEle != 0 &&
		pathEle->GetText() != 0 &&
		strlen( pathEle->GetText() ) != 0 )
		SetShape(pathEle->GetText());
}