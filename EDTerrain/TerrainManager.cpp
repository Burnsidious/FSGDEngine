#include "TerrainManager.h"
#include <fstream>
using namespace std;

#include "Tile.h"

#include "../EDRendererD3D/RenderContext.h"
#include "../EDRendererD3D/RenderShapeWithTexture.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/RasterizerStateManager.h"
#include "../EDRendererD3D/ViewPortManager.h"
using namespace EDRendererD3D;

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

namespace EDTerrain
{
	TerrainManager *TerrainManager::instancePtr = nullptr;

	TerrainManager::TerrainManager(void)
	{
	}

	TerrainManager::~TerrainManager(void)
	{
	}

	TerrainManager *TerrainManager::GetInstance()
	{
		if(nullptr == instancePtr)
			instancePtr = new TerrainManager;
		return instancePtr;
	}

	TerrainManager &TerrainManager::GetReference()
	{
		return *GetInstance();
	}

	void TerrainManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = nullptr;
	}

	void TerrainManager::Initialize(void)
	{
		// Controls the number of patches in a tile
		terrainCBData.gTessellatedTriWidth = 16.0f;
		terrainCBData.gTerrainHeightCoordFactor = 500;
		terrainCBData.gTerrainDiffuseCoordFactor = 15;
		terrainCBData.gTerrainHeightScale = 80.0f;
		terrainCBData.gTerrainHeightOffset = -0.5f;

		terrainCBData.gShowTerrainTransistions = false;
		terrainCBData.gShowTiles = false;
		wireframe = false;

		terrainCBData.gTerrainTest = false;

		innerTileSize = 10;


		CreateRenderContext();
		CreateInputLayout();
		CreateIndices();
		CreateCBuffers();
		CreateTileFrames(5, 16);
		//CreateTileFrames(4, 16);
		//CreateTileFrames(2, 4);
		//CreateTileFrames(1, 32);

		CreateRenderShape();
	}

	void TerrainManager::CreateRenderContext(void)
	{
		// Create the render context to control engine state for terrain rendering
		terrainContextHandle = ContentManager::LoadXML<RenderContext>( 
			"GDRenderContext/rcx_TessellatedTerrain.xml" );
		terrainContextHandle.GetContent()->RenderFunc = TerrainContextRenderFunc;
		terrainContextHandle.GetContent()->SetRenderStage(RenderContext::RS_GBUFFER);
	}

	void TerrainManager::CreateInputLayout(void)
	{
		ifstream load; 	
		load.open("VertexLayout_Terrain_VS.cso", ios_base::binary);
		if(!load.is_open())
			return;
		load.seekg( 0, ios_base::end ); 
		size_t vs_byte_code_size = size_t(load.tellg());
		char *vs_byte_code = new char[vs_byte_code_size];
		load.seekg( 0, ios_base::beg ); 
		load.read( vs_byte_code, vs_byte_code_size );
		load.close();

		const UINT VERTEX_TERRAIN_numElements = 5;
		D3D11_INPUT_ELEMENT_DESC vertexPosDesc[VERTEX_TERRAIN_numElements] =
		{
			{ "POSITION",     0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NEIGHTBOR_SCALE", 0, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NEIGHTBOR_SCALE", 1, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NEIGHTBOR_SCALE", 2, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NEIGHTBOR_SCALE", 3, DXGI_FORMAT_R32_FLOAT, 0, 20, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		HR(Renderer::theDevicePtr->CreateInputLayout(vertexPosDesc, 
			VERTEX_TERRAIN_numElements, vs_byte_code, vs_byte_code_size, &inputLayout));
		delete [] vs_byte_code;
	}

	void TerrainManager::CreateIndices(void)
	{
		const unsigned int VertsPerEdge = 2;
		numIndicies = 4;

		unsigned int index = 0;
		unsigned int *pIndices = new unsigned int[numIndicies];

		// The IB describes one tile of NxN patches.
		for (unsigned int y = 0; y < VertsPerEdge-1; ++y)
		{
			const int rowStart = y * VertsPerEdge;

			for (unsigned int x = 0; x < VertsPerEdge-1; ++x)
			{
				// This order is inportant. If you need to flip the winding you are better off
				// changing the output topology of the hull shader used
				pIndices[index++] = rowStart + x + 1;
				pIndices[index++] = rowStart + x + VertsPerEdge + 1;
				pIndices[index++] = rowStart + x + VertsPerEdge;
				pIndices[index++] = rowStart + x;
			}
		}
		assert(index == numIndicies);

		startingIndex = IndexBuffer::GetReference().AddIndices(pIndices, numIndicies);

		delete[] pIndices;
	}

	void TerrainManager::CreateCBuffers(void)
	{
		// Constant buffer for terrain data
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cbTerrain);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &terrainCBuffer.p));	

		// Constant buffer for individual terrain frame data
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cbPerTileFrame);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(Renderer::theDevicePtr->CreateBuffer(&bd, nullptr, &thePerFrameObjectCBuffer));	
	}

	void TerrainManager::CreateTileFrames(unsigned int numRings, unsigned int ringWidth)
	{
		tileFrames.resize(numRings);
		float tileSize = innerTileSize;
		
		tileFrames[0].Initialize(ringWidth, 0, tileSize);
		tileSize*=2;

		for(unsigned int i = 1; i < numRings; ++i)
		{
			tileFrames[i].Initialize(ringWidth, ringWidth/2, tileSize);
			tileSize*=2;
		}		
	}

	void TerrainManager::CreateRenderShape(void)
	{
		// Have to load a form from file to create terrain render shape
		EDUtilities::ContentHandle<RenderForm> formHandle =
			EDUtilities::ContentManager::LoadXML<RenderForm>(
			"GDForm/terrain/pPlane1/pPlaneShape1/frm_TessellatedTerrain.xml");
		std::vector<EDUtilities::ContentHandle<RenderForm> > forms;
		forms.push_back(formHandle);
		terrainRenderShape.Initialize(forms);
		terrainRenderShape.RenderFunc = TerrainShapeRenderFunc;
		terrainRenderShape.AddToContextSet(terrainContextHandle.GetContent());
	}

	void TerrainManager::AddToRenderSet() 
	{ 
		terrainRenderShape.AddToContextSet(terrainContextHandle.GetContent());
	}

	void TerrainManager::ApplyTextures()
	{
		ID3D11ShaderResourceView *textures[RTT_Count];
		RenderTextureTypes index = RTT_Ambient;
		for( ; index < RTT_Reflected; 
			index = (RenderTextureTypes)(index+1))
		{
			if(terrainRenderShape.GetRenderMaterial()->GetRTHandles()[index].GetContent())
				textures[index] =
				terrainRenderShape.GetRenderMaterial()->GetRTHandles()[index].GetContent()->GetShaderResourceView();
			else
				textures[index] = nullptr;
		}
		Renderer::theContextPtr->PSSetShaderResources(0, 5, textures);
		Renderer::theContextPtr->HSSetShaderResources(0, 5, textures);
		Renderer::theContextPtr->DSSetShaderResources(0, 5, textures);
		Renderer::theContextPtr->VSSetShaderResources(0, 5, textures);
	}

	void TerrainManager::RenderTerrain(void)
	{
		terrainContextHandle.GetContent()->RenderProcess();
	}

	void TerrainManager::UpdateTerrainCBuffer(void)
	{
		const EDMath::Float3 &camPos = ViewPortManager::GetReference().GetActiveViewPosition();

		float stepSize = innerTileSize * 2;

		terrainCBData.gTerrainCameraPosOffsetXZ.x = static_cast<int>(camPos.x / stepSize) * stepSize;
		terrainCBData.gTerrainCameraPosOffsetXZ.y = static_cast<int>(camPos.z / stepSize) * stepSize;

		// write constant buffer data to GPU
		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(terrainCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &terrainCBData, sizeof(terrainCBData));
		Renderer::theContextPtr->Unmap(terrainCBuffer, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cbTerrain::REGISTER_SLOT, 1, &terrainCBuffer.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cbTerrain::REGISTER_SLOT, 1, &terrainCBuffer.p);
		Renderer::theContextPtr->GSSetConstantBuffers(cbTerrain::REGISTER_SLOT, 1, &terrainCBuffer.p);
		Renderer::theContextPtr->DSSetConstantBuffers(cbTerrain::REGISTER_SLOT, 1, &terrainCBuffer.p);
		Renderer::theContextPtr->HSSetConstantBuffers(cbTerrain::REGISTER_SLOT, 1, &terrainCBuffer.p);
	}

	void TerrainManager::UpdatePerTileFrameCBuffer(void)
	{
		D3D11_MAPPED_SUBRESOURCE edit;
		Renderer::theContextPtr->Map(thePerFrameObjectCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, &thePerFrameObjectData, sizeof(thePerFrameObjectData));
		Renderer::theContextPtr->Unmap(thePerFrameObjectCBuffer, 0);

		Renderer::theContextPtr->VSSetConstantBuffers(cbPerTileFrame::REGISTER_SLOT, 1, 
			&thePerFrameObjectCBuffer.p);
		Renderer::theContextPtr->PSSetConstantBuffers(cbPerTileFrame::REGISTER_SLOT, 1, 
			&thePerFrameObjectCBuffer.p);
		Renderer::theContextPtr->GSSetConstantBuffers(cbPerTileFrame::REGISTER_SLOT, 1, 
			&thePerFrameObjectCBuffer.p);
		Renderer::theContextPtr->DSSetConstantBuffers(cbPerTileFrame::REGISTER_SLOT, 1, 
			&thePerFrameObjectCBuffer.p);
		Renderer::theContextPtr->HSSetConstantBuffers(cbPerTileFrame::REGISTER_SLOT, 1, 
			&thePerFrameObjectCBuffer.p);
	}

	void TerrainManager::TerrainShapeRenderFunc(RenderNode &node)
	{
		RenderShapeWithTexture &shape = static_cast<RenderShapeWithTexture &>(node);
		TerrainManager &terrainManager = TerrainManager::GetReference();
		RenderContext &context = *terrainManager.terrainContextHandle.GetContent();

		terrainManager.thePerFrameObjectData.gTileSize = terrainManager.innerTileSize;

		unsigned int numFrames = static_cast<unsigned int>(terrainManager.tileFrames.size());
		for (unsigned int frameIndex = 0; frameIndex < numFrames; ++frameIndex)
		{
			TileFrame &currentTileFrame = terrainManager.tileFrames[frameIndex];

			unsigned int strides[1] = {sizeof(TileInstance)};
			unsigned int offsets[1] = {0};
			Renderer::theContextPtr->IASetVertexBuffers(0, 1, 
				&currentTileFrame.GetVertexBuffer().p, strides, offsets);

			//XMMATRIX world = XMLoadFloat4x4(shape.GetWorldMatrixPtr());
			XMMATRIX viewProj  = XMLoadFloat4x4(
				(XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveViewProjection());
			XMMATRIX view = XMLoadFloat4x4(
				(XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveView());
			XMMATRIX worldViewProj = viewProj;//world*viewProj;
			XMMATRIX worldView = view;//world*view;

			terrainManager.thePerFrameObjectData.gTileProj =
				*(XMFLOAT4X4 *)&ViewPortManager::GetReference().GetActiveProjection();
			XMStoreFloat4x4(&terrainManager.thePerFrameObjectData.gTileWorldView, worldView);
			XMStoreFloat4x4(&terrainManager.thePerFrameObjectData.gTileMVP, worldViewProj);

			terrainManager.UpdatePerTileFrameCBuffer();

			Renderer::theContextPtr->DrawIndexedInstanced(terrainManager.numIndicies, 
				currentTileFrame.GetNumTiles(), terrainManager.startingIndex, 0, 0);

			terrainManager.thePerFrameObjectData.gTileSize *= 2;
		}
	}

	void TerrainManager::TerrainContextRenderFunc(RenderNode &node)
	{
		RenderContext &context = (RenderContext &)node;
		TerrainManager &terrainManager = TerrainManager::GetReference();

		Renderer::theContextPtr->IASetInputLayout(terrainManager.inputLayout);

		Renderer::theContextPtr->IASetIndexBuffer(
			IndexBuffer::GetReference().GetIndices(), DXGI_FORMAT_R32_UINT, 0);

		if(terrainManager.wireframe)
			RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_LINE);

		Renderer::theContextPtr->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		Renderer::theContextPtr->VSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetVertexShader(), NULL, 0 );
		Renderer::theContextPtr->PSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetPixelShader(), NULL, 0 );
		Renderer::theContextPtr->GSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetGeometryShader(), NULL, 0 );
		Renderer::theContextPtr->HSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetHullShader(), NULL, 0 );
		Renderer::theContextPtr->DSSetShader( 
			context.GetRenderStageTechnique()->GetPass(0)->GetDomainShader(), NULL, 0 );

		terrainManager.UpdateTerrainCBuffer();
		terrainManager.ApplyTextures();

		Renderer::Render(context.GetRenderSet());

		if(terrainManager.wireframe)
			RasterizerStateManager::GetReference().ApplyState(RasterizerStateManager::RS_Default);
	}

	void TerrainManager::SetTessTriWidth(float tessTriWidth)
	{
		terrainCBData.gTessellatedTriWidth = tessTriWidth;
		UpdateTerrainCBuffer();
	}

	void TerrainManager::SetHeightMapScale(float heightMapScale)
	{
		terrainCBData.gTerrainHeightScale = heightMapScale;
		UpdateTerrainCBuffer();
	}

	void TerrainManager::SetHeightMapOffset(float heightMapOffset)
	{
		terrainCBData.gTerrainHeightOffset = heightMapOffset;
		UpdateTerrainCBuffer();
	}

	void TerrainManager::ToggleShowTiles(void)
	{
		terrainCBData.gShowTiles = !terrainCBData.gShowTiles;
		UpdateTerrainCBuffer();
	}

	void TerrainManager::ToggleShowTessTransisitions(void)
	{
		terrainCBData.gShowTerrainTransistions = !terrainCBData.gShowTerrainTransistions;
		UpdateTerrainCBuffer();
	}

	void TerrainManager::SetHeightTexCoordScale(float texCoordScale)
	{
		terrainCBData.gTerrainHeightCoordFactor = texCoordScale;
		UpdateTerrainCBuffer();
	}

	void TerrainManager::SetDiffuseTexCoordScale(float texCoordScale)
	{
		terrainCBData.gTerrainDiffuseCoordFactor = texCoordScale;
		UpdateTerrainCBuffer();
	}

}
