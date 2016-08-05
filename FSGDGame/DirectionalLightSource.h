#pragma once

#include "../EDGameCore/Light.h"
#include "../EDRendererD3D/DirectionalLightWithShadow.h"

class DirectionalLightSource : public EDGameCore::DirectionalLight<DirectionalLightSource>
{
public:
	DirectionalLightSource(void);
	DirectionalLightSource(const DirectionalLightSource& rhs);

	~DirectionalLightSource();

	void Render(void);

	EDRendererD3D::DirectionalLightWithShadow *GetLightPtr() { return lightPtr; }

private:
	EDRendererD3D::DirectionalLightWithShadow *lightPtr;

	void LoadState(TiXmlElement* xmlElement);

	void CreateLightPointer();
};