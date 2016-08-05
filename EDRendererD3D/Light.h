#pragma once

namespace EDRendererD3D
{
	class RenderContext;

	class ILight
	{
	public:
		virtual void ApplyLight() = 0;

		virtual std::string GetTechniqueNameOverride() { return ""; }
	};
}
