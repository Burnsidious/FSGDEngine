#pragma once
#include "RenderShapeWithTexture.h"

namespace EDRendererD3D
{
	class RenderShapeGUI :
		public RenderShapeWithTexture
	{
	public:
		RenderShapeGUI(void);
		~RenderShapeGUI(void);

		/// Sets the color to be used when rendering this shape
		/// \param _color - D3DXVECTOR4 defining the color to use
		//inline void SetColor(const D3DXVECTOR4 &_color) { color = _color; }

		/// Sets the color to be used when rendering this shape
		/// \param r - Red componenet ranging from 0.0f to 1.0f
		/// \param g - Green componenet ranging from 0.0f to 1.0f
		/// \param b - Blue componenet ranging from 0.0f to 1.0f
		/// \param a - Alpha componenet ranging from 0.0f to 1.0f
		inline void SetColor(float r, float g, float b, float a = 1.0f) 
		{ 
			/*color.x = r; 
			color.y = g; 
			color.z = b; 
			color.w = a; */
		}

		/// Accessor method for retrieving color of this shape
		/// \return Returns the color used to render this shape
		//inline const D3DXVECTOR4 &GetColor() { return color; }

		static void IndexedGUIPrimitiveRenderFunc(RenderNode &node);

	private:
		/// A color to use when rendering this shape
		//D3DXVECTOR4 color;
	};
}

