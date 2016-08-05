#pragma once



#include "RenderTarget.h"
#include "RenderMesh.h"

namespace EDRendererD3D
{
	enum PostTargetState{ PTS_Default = 0, PTS_DownScale, PTS_UpScale };
	/// An object used to implement an image space post-process effect.
	class PostProcess
	{
	protected:
		/// The pixel shader managed pointer
		CComPtr<ID3D11PixelShader> pixelShader;
		/// The vertex shader managed pointer
		CComPtr<ID3D11VertexShader>	vertexShader;
		/// The mesh that defines the space to render this post-process effect,
		/// typically a full screen quad.
		RenderMesh *meshPtr;

		PostTargetState targetState;

		/// Ignore me!
		void ProcessSolution(RenderTarget &destinationTarget, 
			ID3D11ShaderResourceView *sourceTexture);

		static const int SOURCE_MAP_REGISTER = 8;
	public:
		PostProcess() { }
		virtual ~PostProcess();
		/// Creates the effect to be used by this PostProcess.
        /// \param meshPtr - The address of the mesh to be used while rendering this shape.
		/// \param effectFile - a wide character string defining the name of the 
		/// shader to use for this effect.  A leading L makes an in place string
		/// into a wide character one, ex. L"SomeShader.fx"
		/// \return Returns a buffer to any errors that occured while compiling the shader.
		virtual void Initialize(RenderMesh *_meshPtr, const char *vertexShaderName, 
			const char *pixelShaderName);

		/// Executes the post-process effect.
		virtual void Process(RenderTarget &destinationTarget, ID3D11ShaderResourceView *sourceTexture);

		inline PostTargetState GetTargetState() { return targetState; }
	};
}