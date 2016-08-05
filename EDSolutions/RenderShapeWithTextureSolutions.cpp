#include "precompiled.h"

#include "../EDRendererD3D/RenderShapeWithTexture.h"
#include "../EDRendererD3D/RenderContext.h"
#include "../EDRendererD3D/Renderer.h"

#include "../EDUtilities/PrintConsole.h"
#include "../EDMath/Float3.h"
using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDRendererD3D
{
	void RenderShapeWithTexture::SetTexturesSolution(RenderShapeWithTexture &shape)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderShapeWithTexture::SetTexturesSolution");
			added = true;
		}
		RenderContext &context = *shape.GetRenderContext();

		ID3D11ShaderResourceView *textures[RTT_Count];
		RenderTextureTypes index = RTT_Ambient;
		for( ; index < RTT_Reflected; 
			index = (RenderTextureTypes)(index+1))
		{
			if(shape.GetRenderMaterial()->GetRTHandles()[index].GetContent())
				textures[index] =
				shape.GetRenderMaterial()->GetRTHandles()[index].GetContent()->GetShaderResourceView();
			else
				textures[index] = nullptr;
		}
		if(shape.GetRenderMaterial()->GetRTCubeMapHandle().GetContent())
			textures[index] =
			shape.GetRenderMaterial()->GetRTCubeMapHandle().GetContent()->GetShaderResourceView();
		else
			textures[index] = nullptr;

		context.EffectSetPerObjectTextures(textures);
	}

	void RenderShapeWithTexture::WithTextureIndexedPrimitiveRenderFuncSolution(RenderNode &node)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "Renderer", Float3(1.0f, 0.0f, 0.0f), 
				L"RenderShapeWithTexture::WithTextureIndexedPrimitiveRenderFuncSolution");
			added = true;
		}
		RenderShapeWithTexture &shape = (RenderShapeWithTexture &)node;
		SetTextures(shape);
		RenderShape::IndexedPrimitiveRenderFunc(node);
	}
}
