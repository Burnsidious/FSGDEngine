#include "precompiled.h"
#include "TerrainController.h"

#include "../EDTerrain/TerrainManager.h"
using namespace EDTerrain;

#include "RenderController.h"

TerrainController::TerrainController()
{

}

TerrainController::~TerrainController()
{
	TerrainManager::DeleteInstance();
}

void TerrainController::Awake(void) 
{
	TerrainManager::GetReference().Initialize();

	RenderController::GetInstance()->AddRenderContext(
		TerrainManager::GetReference().GetTerrainContextHandle().GetContent());
}

void TerrainController::Update(void)
{
	TerrainManager::GetReference().AddToRenderSet();
}