#pragma once

namespace EDGameCore
{
	class Camera;

	struct compstruct
	{
		bool operator()(const Camera* a, const Camera* b)const;
	};
}