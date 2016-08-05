#include "precompiled.h"
#include "RenderShapeLight.h"
#include "ViewPortManager.h"
#include "Renderer.h"

#include "VertexBufferManager.h"
#include "IndexBuffer.h"
#include "DepthStencilStateManager.h"
#include "RasterizerStateManager.h"


using namespace DirectX;
using namespace EDUtilities;

namespace EDRendererD3D
{
	RenderShapeLight::RenderShapeLight(void)
	{
	}


	RenderShapeLight::~RenderShapeLight(void)
	{
	}

	void RenderShapeLight::IndexedPrimitiveRenderFunc(RenderNode &node)
	{
		RenderShapeLight &shape = (RenderShapeLight &)node;
		RenderMesh &mesh = *shape.GetRenderMesh();
		RenderContext &context = *shape.GetRenderContext();
		ContentHandle<ShaderEffect> &shaderEffect = context.GetEffect();
		
		const ShaderTechnique *techPtr = 
			shaderEffect.GetContent()->GetTechniqueByName(shape.lightPtr->GetTechniqueNameOverride());
		if(!techPtr)
			return;

		// Set matrices
		XMMATRIX world = XMLoadFloat4x4(shape.GetWorldMatrixPtr());
		XMMATRIX viewProj  = XMLoadFloat4x4((XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection());
		XMMATRIX worldViewProj = world*viewProj;
		Renderer::SetPerObjectData(worldViewProj, world);

		unsigned int passCount = techPtr->GetNumPasses();
		DepthStencilStateManager &dssM = DepthStencilStateManager::GetReference();
		RasterizerStateManager &rsM = RasterizerStateManager::GetReference();

		if(3 == passCount)
		{
			dssM.ApplyState(DepthStencilStateManager::DSS_DeferredLight1);
			rsM.ApplyState(RasterizerStateManager::RS_CCW);
			Renderer::theContextPtr->VSSetShader( 
				techPtr->GetPass(0)->GetVertexShader(), NULL, 0 );
			Renderer::theContextPtr->PSSetShader( 
				techPtr->GetPass(0)->GetPixelShader(), NULL, 0 );
			Renderer::theContextPtr->GSSetShader( 
				techPtr->GetPass(0)->GetGeometryShader(), NULL, 0 );

			Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
				mesh.GetStartIndex(), mesh.GetStartVertex());	

			dssM.ApplyState(DepthStencilStateManager::DSS_DeferredLight2);
			rsM.ApplyState(RasterizerStateManager::RS_Default);
			Renderer::theContextPtr->VSSetShader( 
				techPtr->GetPass(1)->GetVertexShader(), NULL, 0 );
			Renderer::theContextPtr->PSSetShader( 
				techPtr->GetPass(1)->GetPixelShader(), NULL, 0 );
			Renderer::theContextPtr->GSSetShader( 
				techPtr->GetPass(1)->GetGeometryShader(), NULL, 0 );

			Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
				mesh.GetStartIndex(), mesh.GetStartVertex());	

			dssM.ApplyState(DepthStencilStateManager::DSS_LightOutsideFinal);
			Renderer::theContextPtr->VSSetShader( 
				techPtr->GetPass(2)->GetVertexShader(), NULL, 0 );
			Renderer::theContextPtr->PSSetShader( 
				techPtr->GetPass(2)->GetPixelShader(), NULL, 0 );
			Renderer::theContextPtr->GSSetShader( 
				techPtr->GetPass(2)->GetGeometryShader(), NULL, 0 );

			Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
				mesh.GetStartIndex(), mesh.GetStartVertex());	
		}
		else if(2 == passCount)
		{
			dssM.ApplyState(DepthStencilStateManager::DSS_DeferredLight1);
			rsM.ApplyState(RasterizerStateManager::RS_CCW);
			Renderer::theContextPtr->VSSetShader( 
				techPtr->GetPass(0)->GetVertexShader(), NULL, 0 );
			Renderer::theContextPtr->PSSetShader( 
				techPtr->GetPass(0)->GetPixelShader(), NULL, 0 );
			Renderer::theContextPtr->GSSetShader( 
				techPtr->GetPass(0)->GetGeometryShader(), NULL, 0 );

			Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
				mesh.GetStartIndex(), mesh.GetStartVertex());	

			dssM.ApplyState(DepthStencilStateManager::DSS_LightInsideFinal);
			Renderer::theContextPtr->VSSetShader( 
				techPtr->GetPass(1)->GetVertexShader(), NULL, 0 );
			Renderer::theContextPtr->PSSetShader( 
				techPtr->GetPass(1)->GetPixelShader(), NULL, 0 );
			Renderer::theContextPtr->GSSetShader( 
				techPtr->GetPass(1)->GetGeometryShader(), NULL, 0 );

			Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
				mesh.GetStartIndex(), mesh.GetStartVertex());	

			rsM.ApplyState(RasterizerStateManager::RS_Default);
		}
		else
		{
			Renderer::theContextPtr->VSSetShader( 
				techPtr->GetPass(0)->GetVertexShader(), NULL, 0 );
			Renderer::theContextPtr->PSSetShader( 
				techPtr->GetPass(0)->GetPixelShader(), NULL, 0 );
			Renderer::theContextPtr->GSSetShader( 
				techPtr->GetPass(0)->GetGeometryShader(), NULL, 0 );

			Renderer::theContextPtr->DrawIndexed(mesh.GetPrimitiveCount() * 3, 
				mesh.GetStartIndex(), mesh.GetStartVertex());		
		}
	}

	void RenderShapeLight::Initialize(std::vector< ContentHandle<RenderForm> >&formHandles,
		XMFLOAT4X4 *localMatrixPtr)
	{
		RenderShape::Initialize(formHandles, localMatrixPtr);

		RenderFunc = RenderShapeLight::IndexedPrimitiveRenderFunc;
	}
}
