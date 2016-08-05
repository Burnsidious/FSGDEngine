#include "ShapeRenderer.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "RenderController.h"
#include "../EDGameCore/Animation.h"
#include "../EDRendererD3D/RenderShapeSkinned.h"
#include "../EDGameCore/Transform.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Light.h"
#include "PointLightSource.h"
#include "SpotLightSource.h"
#include "DirectionalLightSource.h"

ShapeRenderer::ShapeRenderer(void)
{
	renderShapePtr = 0;
}

ShapeRenderer::ShapeRenderer(const ShapeRenderer& rhs) : EDGameCore::Renderer<ShapeRenderer>(rhs)
{
	renderShapePtr = 0;

	formHandles = rhs.formHandles;

	if( formHandles.front().GetContent() != 0 )
	{
		renderShapePtr = (EDRendererD3D::RenderShape*)formHandles.front().GetContent()->GetNewRenderShape();
		renderShapePtr->Initialize(formHandles);

		RenderController::GetInstance()->AddRenderContext( renderShapePtr->GetRenderContext() );
	}
}

ShapeRenderer::~ShapeRenderer()
{
	delete renderShapePtr;
}

bool ShapeRenderer::CastsShadows(void)const
{
	if( renderShapePtr == 0 )
		return false;

	return renderShapePtr->GetRenderContext()->CastsShadows();
}

const EDMath::Aabb& ShapeRenderer::GetAabb(void)
{
	const EDMath::Float4x4& worldMat = GetGameObject()->GetTransform()->GetWorldMatrix();

	if(renderShapePtr != 0)
	{
		EDMath::Aabb aabb = renderShapePtr->GetRenderMesh()->GetAabb();
		EDMath::TransformAabb(worldAabb, aabb, worldMat);
	}
	else
	{
		worldAabb.center = worldMat.WAxis;
		worldAabb.extents.x = worldAabb.extents.y = worldAabb.extents.z = 0.001f;
	}

	return worldAabb;
}

void ShapeRenderer::SetShape(const std::list< string >& formPaths)
{
	if( renderShapePtr != 0 )
	{
		delete renderShapePtr;
		renderShapePtr = 0;
	}
	
	auto pathIter = formPaths.begin();
	auto pathEnd = formPaths.end();
	for(; pathIter != pathEnd; ++pathIter)
	{
		auto formHandle = EDUtilities::ContentManager::LoadXML<EDRendererD3D::RenderForm>( pathIter->c_str() );

		if( formHandle.GetContent() != 0 )
			formHandles.push_back(formHandle);
	}

	if( !formHandles.empty() )
	{
		renderShapePtr = (EDRendererD3D::RenderShape*)formHandles.front().GetContent()->GetNewRenderShape();
		renderShapePtr->Initialize( formHandles );

		RenderController::GetInstance()->AddRenderContext( renderShapePtr->GetRenderContext() );
	}
}

void ShapeRenderer::Render(void)
{
	return Render(nullptr);
}

void ShapeRenderer::Render(EDRendererD3D::RenderContext *contextPtr)
{
	if( renderShapePtr != 0 )
	{
		const EDMath::Float4x4& worldMat = GetGameObject()->GetTransform()->GetWorldMatrix();

		EDGameCore::Animation* animation = GetAnimation();

		if( animation != 0 )
		{
			EDRendererD3D::RenderShapeSkinned *skinShape = (EDRendererD3D::RenderShapeSkinned *)renderShapePtr;
			skinShape->SetBoneMatrices( animation->GetFlatMats() );
		}
		else
			renderShapePtr->SetWorldMatrix( (DirectX::XMFLOAT4X4*)&worldMat );
		
		renderShapePtr->AddToContextSet(contextPtr);
	}
}

unsigned int ShapeRenderer::GetNumLODs(void)const
{
	if( renderShapePtr == 0 )
		return 0;

	return (unsigned int)renderShapePtr->GetNumLODs();
}

void ShapeRenderer::SetCurrentLOD(unsigned int lod)
{
	if( renderShapePtr )
		renderShapePtr->SetCurrentLOD(lod);
}

void ShapeRenderer::SetZDepth(float z)
{
	if( renderShapePtr )
		renderShapePtr->SetZDepth(z);
}

void ShapeRenderer::LoadState(TiXmlElement* xmlElement)
{
	assert(xmlElement !=  0);
	
	std::list< string > formPaths;

	TiXmlElement* pathEle = xmlElement->FirstChildElement("Path");

	for(; pathEle != 0; pathEle = pathEle->NextSiblingElement("Path"))
	{
		if( pathEle->GetText() != 0 &&
			strlen( pathEle->GetText() ) != 0 )
			formPaths.push_back( string(pathEle->GetText()) );
	}

	if( !formPaths.empty() )
		SetShape( formPaths );
}