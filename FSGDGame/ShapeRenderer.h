#pragma once

#include "../EDGameCore/Renderer.h"
#include "../EDGameCore/Message.h"
#include "../EDRendererD3D/RenderShape.h"

class ShapeRenderer : public EDGameCore::Renderer<ShapeRenderer>
{
public:
	ShapeRenderer(void);
	ShapeRenderer(const ShapeRenderer& rhs);

	~ShapeRenderer();

	const EDMath::Aabb& GetAabb(void);

	//void SetShape(const char* formPath);
	void SetShape(const std::list< string >& formPaths);

	inline EDRendererD3D::RenderShape *GetRenderShape() { return renderShapePtr; }

	void Render(EDRendererD3D::RenderContext *contextPtr);

	bool CastsShadows(void)const;

private:
	unsigned int GetNumLODs(void) const;
	void SetCurrentLOD(unsigned int lod);
	void SetZDepth(float z);

	std::vector< EDUtilities::ContentHandle<EDRendererD3D::RenderForm> > formHandles;

	EDMath::Aabb worldAabb;
	EDRendererD3D::RenderShape *renderShapePtr;

	void Render(void);

	void LoadState(TiXmlElement* xmlElement);
};