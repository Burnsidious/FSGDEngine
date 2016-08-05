#include "precompiled.h"
#include "RenderShapeGUI.h"

namespace EDRendererD3D
{
	RenderShapeGUI::RenderShapeGUI(void)
	{
		SetColor(1, 1, 1, 1);
	}


	RenderShapeGUI::~RenderShapeGUI(void)
	{
	}

	void RenderShapeGUI::IndexedGUIPrimitiveRenderFunc(RenderNode &node)
	{
		//RenderShapeGUI &shape = (RenderShapeGUI &)node;

		//shape.GetRenderContext()->GetEffect()->SetVector("gGUIColor", 
		//	&shape.GetColor());

		return RenderShapeWithTexture::IndexedPrimitiveRenderFunc(node);
	}

}