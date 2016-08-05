#pragma once

#include <d3d11.h>
#include <atlbase.h> 
#include <vector>
#include "TileFrame.h"

#include "../EDRendererD3D/ConstantBuffers.h"
#include "../EDRendererD3D/RenderContext.h"
#include "../EDRendererD3D/RenderShapeWithTexture.h"
#include "../EDUtilities/ContentManager.h"

namespace EDTerrain
{
	class TerrainManager
	{
	public:
		~TerrainManager(void);
		static TerrainManager *GetInstance();
		static TerrainManager &GetReference();

        static void DeleteInstance();

		static void TerrainShapeRenderFunc(EDRendererD3D::RenderNode &node);
		static void TerrainContextRenderFunc(EDRendererD3D::RenderNode &node);

		void Initialize(void);

		void RenderTerrain(void);

		inline void SetWireframe(bool _wireframe) { wireframe = _wireframe; }
		inline bool GetWireframe(void) { return wireframe; }

		void SetTessTriWidth(float tessTriWidth);
		void SetHeightMapScale(float heightMapScale);
		void SetHeightMapOffset(float heightMapOffset);
		void SetHeightTexCoordScale(float texCoordScale);
		void SetDiffuseTexCoordScale(float texCoordScale);
		void ToggleShowTiles(void);
		void ToggleShowTessTransisitions(void);

		void ToggleTerrainTest() { terrainCBData.gTerrainTest = !terrainCBData.gTerrainTest; }

		inline EDUtilities::ContentHandle<EDRendererD3D::RenderContext> GetTerrainContextHandle()
		{return terrainContextHandle;}

		void AddToRenderSet();
	private:

		CComPtr<ID3D11InputLayout> inputLayout;
		CComPtr<ID3D11Buffer> terrainCBuffer;
		cbTerrain terrainCBData; 

		CComPtr<ID3D11Buffer> thePerFrameObjectCBuffer;
		cbPerTileFrame thePerFrameObjectData;

		EDUtilities::ContentHandle<EDRendererD3D::RenderContext> terrainContextHandle;
		EDRendererD3D::RenderShapeWithTexture terrainRenderShape;
		std::vector<TileFrame> tileFrames; 

		unsigned int startingIndex;
		unsigned int numIndicies;
		float innerTileSize;

		// Debug values
		bool wireframe;

		// Internal methods
		void CreateInputLayout(void);
		void CreateIndices(void);
		void CreateCBuffers(void);
		void CreateRenderContext(void);
		void CreateTileFrames(unsigned int numRings, unsigned int ringWidth);
		void CreateRenderShape(void);

		void UpdateTerrainCBuffer(void);
		void UpdatePerTileFrameCBuffer(void);
		void ApplyTextures();
		// Singleton business
		TerrainManager(void);
        TerrainManager(const TerrainManager &) {}
        TerrainManager(const TerrainManager &&) {}
        TerrainManager &operator=(const TerrainManager &) { return *this; }
        TerrainManager &operator=(const TerrainManager &&) { return *this; }

		/// the one and only instance.
        static TerrainManager *instancePtr;
	};
}