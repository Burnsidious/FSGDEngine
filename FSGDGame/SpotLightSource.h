#pragma once

#include "../EDGameCore/Light.h"
#include "../EDRendererD3D/SpotLightWithShadow.h"

class SpotLightSource : public EDGameCore::SpotLight< SpotLightSource >
{
public:
	SpotLightSource(void);
	SpotLightSource(const SpotLightSource& rhs);

	~SpotLightSource(void);

	void Render(void);

	EDRendererD3D::SpotLight *GetLightPtr() { return lightPtr; }

private:
	EDRendererD3D::SpotLight *lightPtr;

	void LoadState(TiXmlElement* xmlElement);

	void CreateLightPointer();
};
