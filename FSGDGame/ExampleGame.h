#pragma once

#include "../EDGameCore/Game.h"

namespace EDRendererD3D
{
	class RenderContext;
}

class ExampleGame : public EDGameCore::Game
{
public:
	void Initialize(void);
	void RegisterTypes(void);
	void RegisterStrings(void);

	void ProcessCamera(void);
	void ProcessLight(EDGameCore::ILight* light);
	void ProcessVisibleLightSet(void);
	void ProcessRendererPostLit(EDGameCore::IRenderer* renderer);
	void ProcessRendererPreLit(EDGameCore::IRenderer* renderer);
	void PostProcessCamera(void);
	void ProcessPreLitVisibleSet(void); 
	void ProcessPostLitVisibleSet(void);

	void BuildLightViewSet(EDGameCore::ILight* light);


private:

};