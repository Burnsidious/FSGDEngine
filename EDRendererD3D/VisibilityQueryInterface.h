#pragma once

#include "RenderTarget.h"
#include "RenderShape.h"

namespace EDRendererD3D
{
	enum VisibilityQueryResult{ VQR_Incomplete = 0, VQR_Visible, VQR_NotVisiable };
	class VisibilityQueryInterface
	{
	public:
		VisibilityQueryInterface(void);
		~VisibilityQueryInterface(void);

		VisibilityQueryResult GetResult(void);
		static RenderShapeBase *theShapePtr;
	private:

		RenderTarget *renderTargetPtr;

		DWORD pixelCountThreshold;
	};
}
