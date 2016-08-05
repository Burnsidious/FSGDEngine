#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;


namespace EDRendererD3D
{
	enum VertexFormat{ eVERTEX_POS = 0, eVERTEX_POSCOLOR, eVERTEX_POSTEX, eVERTEX_POSNORMTEX, 
		eVERTEX_POSNORMTANTEX, eVERTEX_POSBONEWEIGHT, eVERTEX_POSBONEWEIGHTNORMTEX, 
		eVERTEX_POSBONEWEIGHTNORMTANTEX, eVERTEX_MAX };

	extern const char* VertexFormatString[eVERTEX_MAX];

	/// A vertex structure used to represent vertices containing only positional data.
	struct VERTEX_POS
	{
		DirectX::XMFLOAT3 position;  // 12
	};

	/// A vertex structure used to represent vertices containing position and color data.
	struct VERTEX_POSCOLOR
	{
		DirectX::XMFLOAT3 position;  // 12
		DirectX::XMFLOAT4 color;	   // 16
	};

	/// A vertex structure used to represent vertices containing position and texture coordinate data.
	struct VERTEX_POSTEX
	{
		DirectX::XMFLOAT3 position;  // 12
		DirectX::XMFLOAT2 texcoord;  // 8
	};

	/// A vertex structure used to represent vertices containing position, normal and texture coordinate data.
	struct VERTEX_POSNORMTEX
	{
		DirectX::XMFLOAT3 position;  // 12
		DirectX::XMFLOAT3 normal;  // 12
		DirectX::XMFLOAT2 texcoord;  // 8
	};

	/// A vertex structure used to represent vertices containing position, normal and texture coordinate data.
	struct VERTEX_POSNORMTANTEX
	{
		DirectX::XMFLOAT3 position;  // 12
		DirectX::XMFLOAT3 normal;  // 12
		DirectX::XMFLOAT2 texcoord;  // 8
		DirectX::XMFLOAT3 tangent; // 12
		float			  determinant; //4  
	};

	struct VERTEX_POSBONEWEIGHT
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMINT4   bone;
		DirectX::XMFLOAT4 weights;
	};

	struct VERTEX_POSBONEWEIGHTNORMTEX
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMINT4   bone;
		DirectX::XMFLOAT4 weights;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct VERTEX_POSBONEWEIGHTNORMTANTEX
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		DirectX::XMINT4   bone;
		DirectX::XMFLOAT4 weights;
		DirectX::XMFLOAT3 tangent;
		float			  determinant; 
	};

	class InputLayoutManager
	{
	public:

		~InputLayoutManager(void);
		/// Returns a reference to the one instance of this class.
        static InputLayoutManager &GetReference();
        /// Deletes the one reference to this class.
        static void DeleteInstance();

		ID3D11InputLayout *GetInputLayout(VertexFormat index) { return inputLayouts[index]; }
	private:
        /// Private constructor to ensure singleton structure.
        InputLayoutManager(void);
        /// Private copy constructor to ensure singleton structure.
        InputLayoutManager(const InputLayoutManager &) {}
        /// Private move constructor to ensure singleton structure.
        InputLayoutManager(const InputLayoutManager &&) {}
        /// Private assignment operator to ensure singleton structure.
        InputLayoutManager &operator=(const InputLayoutManager &) {}
        /// Private move operator to ensure singleton structure.
        InputLayoutManager &operator=(const InputLayoutManager &&) {}

        /// The one and only instance of this class.
		static InputLayoutManager * instancePtr;

		/// The input layout interfaces for each of the supported vertex formats
		ID3D11InputLayout * inputLayouts[eVERTEX_MAX];

		/// Private method used to create all input layout interfaces
		void Initialize(void);
	};
}