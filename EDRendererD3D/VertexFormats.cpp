#include "precompiled.h"
#include "VertexFormats.h"
#include "Renderer.h"

#include "../EDUtilities/InternalOutput.h"
using namespace EDUtilities;

using std::ifstream;
using std::ios_base;

const char* EDRendererD3D::VertexFormatString[] = { "VERTEX_POS", "VERTEX_POSCOLOR", "VERTEX_POSTEX", 
	"VERTEX_POSNORMTEX", "VERTEX_POSNORMTANTEX", "VERTEX_POSBONEWEIGHT", "VERTEX_POSBONEWEIGHTNORMTEX", 
	"VERTEX_POSBONEWEIGHTNORMTANTEX"};

namespace EDRendererD3D
{
	InputLayoutManager *InputLayoutManager::instancePtr = 0;

	InputLayoutManager::InputLayoutManager(void) 
	{
		for(VertexFormat index = VertexFormat(0); index < eVERTEX_MAX; index = VertexFormat(index + 1))
			inputLayouts[index] = 0;
	}

	InputLayoutManager::~InputLayoutManager(void) 
	{
		for(VertexFormat index = VertexFormat(0); index < eVERTEX_MAX; index = VertexFormat(index + 1))
			ReleaseCOM(inputLayouts[index]);
	}
	
	InputLayoutManager &InputLayoutManager::GetReference()
	{
		if(0 == instancePtr)
		{
			instancePtr = new InputLayoutManager;
			instancePtr->Initialize();
		}
		return *instancePtr;
	}
	void InputLayoutManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = 0;
	}

	void InputLayoutManager::Initialize()
	{
		ifstream load; 	
		load.open("VertexLayout_POS_VS.cso", ios_base::binary);
		//load.open("VertexLayout_POS_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		size_t vs_byte_code_size = size_t(load.tellg());
		char *vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();

		// VERTEX_POS
		const UINT VERTEX_POS_numElements = 1;
		D3D11_INPUT_ELEMENT_DESC vertexPosDesc[VERTEX_POS_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosDesc, VERTEX_POS_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POS]));
		delete [] vs_byte_code;

		// VERTEX_POSCOLOR
		load.open("VertexLayout_POSCOLOR_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();
		
		const UINT VERTEX_POSCOLOR_numElements = 2;
		D3D11_INPUT_ELEMENT_DESC vertePosColorxDesc[VERTEX_POSCOLOR_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertePosColorxDesc, VERTEX_POSCOLOR_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSCOLOR]));
		delete [] vs_byte_code;

		//VERTEX_POSTEX
		load.open("VertexLayout_POSTEX_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();
		
		const UINT VERTEX_POSTEX_numElements = 2;
		D3D11_INPUT_ELEMENT_DESC vertexPosTexDesc[VERTEX_POSTEX_numElements] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosTexDesc, VERTEX_POSTEX_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSTEX]));
		delete [] vs_byte_code;

		//VERTEX_POSNORMTEX
		load.open("VertexLayout_POSNORMTEX_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();
		
		const UINT VERTEX_POSNORMTEX_numElements = 3;
		D3D11_INPUT_ELEMENT_DESC vertexPosNormTexDesc[VERTEX_POSNORMTEX_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosNormTexDesc, VERTEX_POSNORMTEX_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSNORMTEX]));
		delete [] vs_byte_code;

		//VERTEX_POSNORMTANTEX
		load.open("VertexLayout_POSNORMTANTEX_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();

		const UINT VERTEX_POSNORMTANTEX_numElements = 5;
		D3D11_INPUT_ELEMENT_DESC vertexPosNormTanTexDesc[VERTEX_POSNORMTANTEX_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosNormTanTexDesc, VERTEX_POSNORMTANTEX_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSNORMTANTEX]));
		delete [] vs_byte_code;

		//VERTEX_POSBONEWEIGHT
		load.open("VertexLayout_POSBONEWEIGHT_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();

		const UINT VERTEX_POSBONEWEIGHT_numElements = 3;
		D3D11_INPUT_ELEMENT_DESC vertexPosBoneWeightDesc[VERTEX_POSBONEWEIGHT_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosBoneWeightDesc, 
			VERTEX_POSBONEWEIGHT_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSBONEWEIGHT]));
		delete [] vs_byte_code;


		//VERTEX_POSBONEWEIGHTNORMTEX
		load.open("VertexLayout_POSBONEWEIGHTNORMTEX_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();

		const UINT VERTEX_POSBONEWEIGHTNORMTEX_numElements = 5;
		D3D11_INPUT_ELEMENT_DESC vertexPosBoneWeightNormTexDesc[VERTEX_POSBONEWEIGHTNORMTEX_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosBoneWeightNormTexDesc, 
			VERTEX_POSBONEWEIGHTNORMTEX_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSBONEWEIGHTNORMTEX]));
		delete [] vs_byte_code;

		//VERTEX_POSBONEWEIGHTNORMTANTEX
		load.open("VertexLayout_POSBONEWEIGHTNORMTANTEX_VS.cso", ios_base::binary);
		load.seekg( 0, ios_base::end ); 
		vs_byte_code_size = size_t(load.tellg());
		vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();

		const UINT VERTEX_POSBONEWEIGHTNORMTANTEX_numElements = 7;
		D3D11_INPUT_ELEMENT_DESC vertexPosBoneWeightNormTanTexDesc[VERTEX_POSBONEWEIGHTNORMTANTEX_numElements] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosBoneWeightNormTanTexDesc, VERTEX_POSBONEWEIGHTNORMTANTEX_numElements, vs_byte_code, 
			vs_byte_code_size, &inputLayouts[eVERTEX_POSBONEWEIGHTNORMTANTEX]));
		delete [] vs_byte_code;
	}
}