#pragma once

#include "../EDGameCore/SkyBox.h"
#include "../EDRendererD3D/RenderShape.h"

class SkyBox : public EDGameCore::SkyBoxT<SkyBox>
{
private:

	EDRendererD3D::RenderShape* renderShape;
	EDUtilities::ContentHandle<EDRendererD3D::RenderForm> formHandle;

public:
	SkyBox(void);
	SkyBox(const SkyBox& rhs);
	virtual ~SkyBox();

	void SetShape(const char* formPath);
	void LoadState(TiXmlElement* xmlElement);

	void Render(void);
};