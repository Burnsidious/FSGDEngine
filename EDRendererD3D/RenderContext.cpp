#include "precompiled.h"
#include "RenderContext.h"
#include "RenderContextWithTexture.h"
#include "Renderer.h"
#include "VertexBufferManager.h"
#include "IndexBuffer.h"
#include "RenderMaterial.h"
#include "ViewPortManager.h"
#include "RasterizerStateManager.h"
#include "BlendStateManager.h"
#include "GraphicsProfiler.h"
#include "RenderShapeSkinned.h"


#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
#include "../tinyxml/tinyxml.h"

using namespace EDUtilities;
using namespace std;


namespace EDRendererD3D
{
	RenderContext::RenderContext()
	{
		renderSetPtr = 0;

		currentStage = RS_UNLIT;
		memset(renderStageTechniques, 0, sizeof(ShaderTechnique *) * RS_MAX);

		castShadows = false;
	}
	RenderContext::~RenderContext()
	{
		delete renderSetPtr;
	}

	bool RenderContext::CreateEffect(const char *effectFile)
	{
		effectPtr = ContentManager::LoadXML<ShaderEffect>(effectFile);
		
		return true;
	}

	void RenderContext::ContextSharedRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextSharedRenderFuncSolution(node);
	}

	void RenderContext::ContextPosNormalUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPosNormalUVRenderFuncSolution(node);
	}

	void RenderContext::ContextTransparentPosNormalUVRenderFunc(RenderNode &node)
	{
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_CCW);
		ContextPosNormalUVRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
		ContextPosNormalUVRenderFunc(node);
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	}

	void RenderContext::ContextPosNormalUVNoCullRenderFunc(RenderNode &node)
	{
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_NOCULL);
		return ContextPosNormalUVRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
	}

	void RenderContext::ContextPosNormalTangentUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPosNormalTangentUVRenderFuncSolution(node);
	}

	void RenderContext::ContextTransparentPosNormalTangentUVRenderFunc(RenderNode &node)
	{
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_CCW);
		ContextPosNormalTangentUVRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
		ContextPosNormalTangentUVRenderFunc(node);
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	}

	void RenderContext::ContextPosNormalTangentUVNoCullRenderFunc(RenderNode &node)
	{
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_NOCULL);
		return ContextPosNormalTangentUVRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
	}

	void RenderContext::ContextPositionRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPositionRenderFuncSolution(node);
	}

	void RenderContext::ContextTransparentPositionRenderFunc(RenderNode &node)
	{
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_CCW);
		ContextPositionRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
		ContextPositionRenderFunc(node);
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	}

	void RenderContext::ContextPositionUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPositionUVRenderFuncSolution(node);
	}

	void RenderContext::ContextTransparentPositionUVRenderFunc(RenderNode &node)
	{
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_CCW);
		ContextPositionUVRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
		ContextPositionUVRenderFunc(node);
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	}

	void RenderContext::ContextPositionColorRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_NOCULL);

		return ContextPositionColorRenderFuncSolution(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);

	}

	void RenderContext::ContextTransparentPositionColorRenderFunc(RenderNode &node)
	{
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Alpha);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_CCW);
		ContextPositionColorRenderFunc(node);
		RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
		ContextPositionColorRenderFunc(node);
		BlendStateManager::GetReference().ApplyState(BlendStateManager::BS_Default);
	}

	void RenderContext::ContextPosBoneWeightNormalUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPosBoneWeightNormalUVRenderFuncSolution(node);
	}

	void RenderContext::ContextPosBoneWeightNormalTanUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPosBoneWeightNormalTanUVRenderFuncSolution(node);
	}

	void RenderContext::SetRenderStageTechnique(TiXmlElement *elementPtr, 
		const char *techName, const char *xmlFileNamePtr, RenderStage stage)
	{
		TiXmlElement* techElePtr = elementPtr->FirstChildElement(techName);
		if( techElePtr != 0 )
		{
			if( techElePtr->GetText() != 0 )
			{
				std::string technique = techElePtr->GetText();
				if( technique.length() != 0 )
				{
					ShaderTechnique *techPtr = effectPtr.GetContent()->GetTechniqueByName(technique.c_str());
					if(!techPtr)
					{
						InternalOutput::GetReference().Error(
							"Context %s specifies %s, but the technique does not exist in the shader\n", 
							xmlFileNamePtr, techName);

					}
					SetRenderStageTechnique( stage, techPtr);
				}
			}
		}
	}

	RenderContext* RenderContext::LoadXML(const char* xmlFileNamePtr, const char* indentifier)
	{
		TiXmlDocument doc((ContentManager::theContentPath + xmlFileNamePtr).c_str());

		if(!doc.LoadFile())
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
			return 0;
		}

		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;
		TiXmlHandle hRoot(0);

		elementPtr = hDoc.FirstChildElement().Element();
		if( !elementPtr )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, is the file empty?\n", xmlFileNamePtr);
			return 0;
		}

		std::string rootName = elementPtr->Value();

		if( strcmp(rootName.c_str(), "GDRenderContext") != 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GDRenderContext element\n", 
				xmlFileNamePtr);
			return 0;
		}

		TiXmlElement* pMaterialElement = elementPtr->FirstChildElement("Material");

		std::string materialString;
		std::string shaderEffectString;
		std::string techniqueString;
		std::string vertexFormatString;
		std::string castShadowString;
		std::string renderStyleString;

		if( pMaterialElement != 0 )
		{
			if(pMaterialElement->GetText() != 0)
				materialString = pMaterialElement->GetText();
		}

		// Error check for old Effect based shader usage
		TiXmlElement* pEffectElement = elementPtr->FirstChildElement("Effect");
		if(nullptr == pEffectElement )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing Effect element", xmlFileNamePtr);
			return 0;
		}
		shaderEffectString = pEffectElement->GetText();

		TiXmlElement* pTechniqueElement = elementPtr->FirstChildElement("Technique");

		if( pTechniqueElement != 0 )
		{
			if( pTechniqueElement->GetText() != 0 )
				techniqueString = pTechniqueElement->GetText();
		}

		if( techniqueString.length() == 0 )
			techniqueString = "Basic";

		TiXmlElement* pVertexFormatElement = elementPtr->FirstChildElement("VertexFormat");

		if( pVertexFormatElement == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a VertexFormat element\n", xmlFileNamePtr);
			return 0;
		}

		vertexFormatString = pVertexFormatElement->GetText();

		bool isDeferredPointLightContext = false;
		TiXmlElement* pPointLightTechnique = elementPtr->FirstChildElement("PointLightTechnique");
		if( pPointLightTechnique != 0 )
		{
			if( pPointLightTechnique->GetText() != 0 )
			{
				std::string technique = pPointLightTechnique->GetText();
				if( technique.length() != 0 )
					isDeferredPointLightContext = true;
			}

		}	
		bool isDeferredDirLightContext = false;
		TiXmlElement* pDirLightTechnique = elementPtr->FirstChildElement("DirLightTechnique");
		if( pDirLightTechnique != 0 )
		{
			if( pDirLightTechnique->GetText() != 0 )
			{
				std::string technique = pDirLightTechnique->GetText();
				if( technique.length() != 0 )
					isDeferredDirLightContext = true;
			}
		}
		bool isDeferredSpotLightContext = false;
		TiXmlElement* pSpotLightTechnique = elementPtr->FirstChildElement("SpotLightTechnique");
		if( pSpotLightTechnique != 0 )
		{
			if( pSpotLightTechnique->GetText() != 0 )
			{
				std::string technique = pSpotLightTechnique->GetText();
				if( technique.length() != 0 )
					isDeferredSpotLightContext = true;
			}
		}

		bool castShadows = false;
		TiXmlElement* pCastShadowsEle = elementPtr->FirstChildElement("CastShadows");
		if( pCastShadowsEle != 0 )
		{
			castShadows = true;

			if( pCastShadowsEle->GetText() != 0 )
			{
				if( strlen( pCastShadowsEle->GetText() ) != 0 &&
					(_stricmp( pCastShadowsEle->GetText(), "false") == 0 ||
					strcmp( pCastShadowsEle->GetText(), "0" ) == 0 ) )
				{
					castShadows = false;
				}
			}
		}

		TiXmlElement* pRenderStyleElement = elementPtr->FirstChildElement("RenderStyle");

		renderStyleString = pRenderStyleElement->GetText();

		RenderContext* pRenderContext = 0;

		if( materialString.length() != 0 )
		{
			RenderContextWithTexture* pRcWithTex = new RenderContextWithTexture;
			pRenderContext = pRcWithTex;

			if( strcmp( vertexFormatString.c_str(), "VERTEX_POS" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPositionRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POS;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSCOLOR" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPositionRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POSCOLOR;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPositionUVRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POSTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPosNormalUVRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POSNORMTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTANTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPosNormalTangentUVRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POSNORMTANTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSBONEWEIGHTNORMTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPosBoneWeightNormalUVRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POSBONEWEIGHTNORMTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSBONEWEIGHTNORMTANTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPosBoneWeightNormalTanUVRenderFunc;
				pRcWithTex->vertexFormat = eVERTEX_POSBONEWEIGHTNORMTANTEX;
			}
		}
		else
		{
			pRenderContext = new RenderContext;

			if( "TRANSPARENT" == renderStyleString )
			{
				if( strcmp( vertexFormatString.c_str(), "VERTEX_POS" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextTransparentPositionRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POS;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSCOLOR" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextTransparentPositionColorRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSCOLOR;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextTransparentPositionUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSTEX;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextTransparentPosNormalUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSNORMTEX;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTANTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextTransparentPosNormalTangentUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSNORMTANTEX;
				}
			}
			else
			{
				if( strcmp( vertexFormatString.c_str(), "VERTEX_POS" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPositionRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POS;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSCOLOR" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPositionColorRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSCOLOR;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPositionUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSTEX;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPosNormalUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSNORMTEX;
				}
				else if (strcmp(vertexFormatString.c_str(), "VERTEX_POSNORMTEXNOCULL") == 0)
				{
					pRenderContext->RenderFunc = ContextPosNormalUVNoCullRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSNORMTEX;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTANTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPosNormalTangentUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSNORMTANTEX;
				}
				else if (strcmp(vertexFormatString.c_str(), "VERTEX_POSNORMTANTEXNOCULL") == 0)
				{
					pRenderContext->RenderFunc = ContextPosNormalTangentUVNoCullRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSNORMTANTEX;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSBONEWEIGHTNORMTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPosBoneWeightNormalUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSBONEWEIGHTNORMTEX;
				}
				else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSBONEWEIGHTNORMTANTEX" ) == 0 )
				{
					pRenderContext->RenderFunc = ContextPosBoneWeightNormalTanUVRenderFunc;
					pRenderContext->vertexFormat = eVERTEX_POSBONEWEIGHTNORMTANTEX;
				}
			}
		}

		if( materialString.length() != 0 )
		{
			//std::string materialFilePath = "Resources/GDAssets";
			//materialFilePath += materialString;

			//ContentHandle<RenderMaterial> matHandle = ContentManager::LoadXML<RenderMaterial>(materialFilePath.c_str());
		}

		pRenderContext->castShadows = castShadows;

		// Set up shaders
		if(!pRenderContext->CreateEffect((string("ShaderEffects/") + shaderEffectString).c_str()))
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, shader effect, %s, failed to load\n", xmlFileNamePtr, 
				shaderEffectString.c_str());
		}

		ShaderTechnique *techPtr = nullptr;

		pRenderContext->SetRenderStageTechnique(elementPtr, "GBufferTechnique", xmlFileNamePtr, 
			RenderContext::RS_GBUFFER);

		pRenderContext->SetRenderStageTechnique(elementPtr, "DepthCameraTechnique", xmlFileNamePtr, 
			RenderContext::RS_DEPTH_CAMERA);

		pRenderContext->SetRenderStageTechnique(elementPtr, "DepthSpotLightTechnique", xmlFileNamePtr, 
			RenderContext::RS_DEPTH_SPT_LIGHT);

		TiXmlElement* pDepthPointLightTechnique = elementPtr->FirstChildElement("DepthPointLightTechnique");
		if( pDepthPointLightTechnique != 0 )
		{
			if( pDepthPointLightTechnique->GetText() != 0 )
			{
				std::string technique = pDepthPointLightTechnique->GetText();
				if( technique.length() != 0 )
				{
					ShaderTechnique *techPtr = pRenderContext->effectPtr.GetContent()->GetTechniqueByName(
						technique.c_str());
					if(!techPtr)
					{
						InternalOutput::GetReference().Error(
							"Context %s specifies %s, but the technique does not exist in the shader\n", 
							xmlFileNamePtr, "DepthPointLightTechnique");

					}
					pRenderContext->SetRenderStageTechnique( 
						RenderStage(RenderContext::RS_DEPTH_PNT_LIGHT), techPtr);
				}
			}
		}

		pRenderContext->SetRenderStageTechnique(elementPtr, "DepthDirectionalLightTechnique", xmlFileNamePtr, 
			RenderContext::RS_DEPTH_DIR_LIGHT);

		pRenderContext->SetRenderStageTechnique(elementPtr, "UnlitTechnique", xmlFileNamePtr, 
			RenderContext::RS_UNLIT);

		pRenderContext->SetRenderStageTechnique(elementPtr, "TransparencyTechnique", xmlFileNamePtr, 
			RenderContext::RS_TRANSPARENT);

		pRenderContext->SetRenderStageTechnique(elementPtr, "GUITechnique", xmlFileNamePtr, 
			RenderContext::RS_GUI);

		if( isDeferredPointLightContext )
		{
			std::string technique = pPointLightTechnique->GetText();
			if( technique.length() != 0 )
				pRenderContext->SetRenderStageTechnique( RenderContext::RS_PNT_LIGHT, 
					pRenderContext->effectPtr.GetContent()->GetTechniqueByName(technique.c_str() ));
		}

		if( isDeferredDirLightContext )
		{
			std::string technique = pDirLightTechnique->GetText();
			if( technique.length() != 0 )
				pRenderContext->SetRenderStageTechnique( RenderContext::RS_DIR_LIGHT, 
					pRenderContext->effectPtr.GetContent()->GetTechniqueByName(technique.c_str() ));
		}

		if( isDeferredSpotLightContext )
		{
			std::string technique = pSpotLightTechnique->GetText();
			if( technique.length() != 0 )
				pRenderContext->SetRenderStageTechnique( RenderContext::RS_SPT_LIGHT, 
					pRenderContext->effectPtr.GetContent()->GetTechniqueByName(technique.c_str() ));
		}

		////////////////////////////////////////////////////////////////////////////////////
		// Layout verification
		//LayoutVerification((string("ShaderEffects/") + shaderEffectString).c_str(), vertexFormatString);
		
		// End layout verification

		if( pRenderContext->GetRenderStageTechnique( RenderContext::RS_TRANSPARENT ) != 0 )
			pRenderContext->CreateRenderSet( true, RenderSetSorted::ZSortSmaller );
		else
			pRenderContext->CreateRenderSet();

		return pRenderContext;
	}
}