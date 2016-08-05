#pragma once

#include "../EDGameCore/Light.h"
#include "../EDRendererD3D/PointLightWithShadow.h"

class PointLightSource : public EDGameCore::PointLight<PointLightSource>
{
public:
	PointLightSource(void);
	PointLightSource(const PointLightSource& rhs);

	~PointLightSource();

	void Render(void);

	EDRendererD3D::PointLight *GetLightPtr() { return lightPtr; }

private:
	EDRendererD3D::PointLight *lightPtr;

	void LoadState(TiXmlElement* xmlElement);

	void CreateLightPointer();
};