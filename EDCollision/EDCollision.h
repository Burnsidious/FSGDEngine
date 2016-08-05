#pragma once

#include <algorithm>
#include "../EDMath/float3.h"
#include "../EDMath/boundingVolume.h"

using namespace EDMath;
using namespace std;

// Machine epsilon. Smallest value where guaranteed that: A + FLT_EPSILON != A
#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif

#ifndef ED_EPSILON
#define ED_EPSILON 3.45266983072520e-04F
#endif

namespace EDCollision
{
	struct Contact
	{
		static const unsigned int CONTACT_MAX = 12;

		Float3 Point[CONTACT_MAX];
		Float3 Normal;
		float PenetrationDepth;
		unsigned int numPoints;

		void negate(void);
		void getClosestPoint(const Float3& pt, Float3& c)const;
	};

	int PointOutsideOfPlane(Float3 p, Float3 a, Float3 b, Float3 c, Float3 d);

	float ClosestPtPointTetrahedron(Float3 p, Float3 a, Float3 b, Float3 c, Float3 d, Float3& closestPt);

	inline float Clamp(float v, float m, float M){ return min(M, max(m, v)); }

	bool Collide(const EDMath::BoundingVolume* first, const EDMath::BoundingVolume* second, Contact* contact = 0);

	bool CollideTriangle(const EDMath::BoundingVolume& boundingVolume, const Float3& a, const Float3& b, const Float3& c, Contact* contact = 0);
}