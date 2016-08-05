#include "obb.h"
#include "aabb.h"

namespace EDMath
{
	const Float3 Obb::unitVertices[8] =
	{
		Float3( -1.0f, -1.0f, -1.0f ),
		Float3( -1.0f, -1.0f, 1.0f ),
		Float3( -1.0f, 1.0f, -1.0f ),
		Float3( -1.0f, 1.0f, 1.0f ),
		Float3( 1.0f, -1.0f, -1.0f ),
		Float3( 1.0f, -1.0f, 1.0f ),
		Float3( 1.0f, 1.0f, -1.0f ),
		Float3( 1.0f, 1.0f, 1.0f )
	};

	const unsigned int Obb::faceIndices[6][4] =
	{		
		{0,1,3,2},
		{1,0,4,5},
		{0,2,6,4},
		{4,6,7,5},
		{2,3,7,6},
		{1,5,7,3}
	};

	void ComputeObb(Obb& obb, const Float3 pts[], unsigned int numPts)
	{
		Aabb aabb;
		ComputeAabb( aabb, pts, numPts );

		obb.transform.makeIdentity();
		obb.transform.WAxis = aabb.center;
		obb.extents = aabb.extents;
	}

	void TransformObb(Obb& out, const Obb& in, const Float4x4& transform)
	{
		Float4x4 result;
		Multiply( result, in.transform, transform );

		out.extents = in.extents;
		out.transform = result;
	}

	void InvTransformObb(Obb& out, const Obb& in, const Float4x4& transform)
	{
		out.extents = in.extents;
		InvTransformPoint( out.transform.WAxis, in.transform.WAxis, transform );
		out.transform.padW = 1.0f;
		
		InvTransformVector( out.transform.XAxis, in.transform.XAxis, transform );
		out.transform.padX = 0.0f;

		InvTransformVector( out.transform.YAxis, in.transform.YAxis, transform );
		out.transform.padY = 0.0f;

		InvTransformVector( out.transform.ZAxis, in.transform.ZAxis, transform );
		out.transform.padZ = 0.0f;
	}

	Aabb Obb::GetAabb(void)const
	{
		Aabb aabb;

		for(int i = 0; i < 3; ++i)
		{
			aabb.center.v[i] = transform.WAxis.v[i];
			aabb.extents.v[i] = 0.0f;

			for(int j = 0; j < 3; ++j)
				aabb.extents.v[i] += abs( transform.m[j][i] ) * extents.v[j];
		}

		return aabb;
	}
}