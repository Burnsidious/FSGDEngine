#pragma once

namespace EDMath
{
	struct Aabb;

	struct BoundingVolume
	{
		enum Type{ SPHERE, CAPSULE, AABB, OBB };
		virtual Type GetType(void)const = 0;
		virtual Aabb GetAabb(void)const = 0;
	};
}