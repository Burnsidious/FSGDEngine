#pragma once
#include "rendershape.h"
#include "Light.h"

namespace EDRendererD3D
{
	class RenderShapeLight :
		public RenderShape
	{
	public:
		RenderShapeLight(void);
		~RenderShapeLight(void);

		inline void SetLight(ILight *_lightPtr) { lightPtr = _lightPtr; }

		/// An EDRenderFunc that can render shapes whose vertices are indexed, and must apply a light
		static void IndexedPrimitiveRenderFunc(RenderNode &node);

		virtual void Initialize(
			std::vector< EDUtilities::ContentHandle<RenderForm> > &formHandles, 
			DirectX::XMFLOAT4X4 *localMatrixPtr = 0 );
	private:
		ILight *lightPtr;
	};
}
