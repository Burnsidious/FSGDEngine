#include "precompiled.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDMath/aabb.h"
#include "../EDMath/obb.h"
#include "../EDMath/capsule.h"
#include "../EDMath/polygon.h"
#include "../EDCollision/EDCollision.h"
#include "../EDCollision/segment.h"
#include "../EDRendererD3D/DebugRenderer.h"

using EDUtilities::PrintConsole;
using EDMath::Float3;

namespace EDCollision
{
	bool ObbToAabbFaceContact(const Obb& obb, const Aabb& aabb, int nAxis, Float3& contactPt)
	{
		Float3 vAxis(0.0f, 0.0f, 0.0f);
		Float3 t = obb.transform.WAxis - aabb.center;
		
		vAxis.v[nAxis] = t.v[nAxis] < 0.0f ? -1.0f : 1.0f;

		Float3 localAxis;
		InvTransformVector( localAxis, vAxis, obb.transform );

		Float3 scaledAxis( -localAxis.x / obb.extents.x, -localAxis.y / obb.extents.y, -localAxis.z / obb.extents.z );

		int faceAxis = 0;
		if( abs(scaledAxis.x) < abs(scaledAxis.y) )
			faceAxis = 1;
		if( abs(scaledAxis.v[faceAxis]) < abs(scaledAxis.z) )
			faceAxis = 2;

		Float3 verts[12];
		int faceIndex = scaledAxis.v[faceAxis] < 0.0f ? faceAxis : faceAxis + 3;

		for(int i = 0; i < 4; ++i)
		{
			verts[i].x = Obb::unitVertices[ Obb::faceIndices[faceIndex][i] ].x * obb.extents.x;
			verts[i].y = Obb::unitVertices[ Obb::faceIndices[faceIndex][i] ].y * obb.extents.y;
			verts[i].z = Obb::unitVertices[ Obb::faceIndices[faceIndex][i] ].z * obb.extents.z;

			TransformPoint( verts[i], verts[i], obb.transform );
			verts[i] -= aabb.center;
		}

		int clipAxes[][2] = { {1, 2}, {0, 2}, {0, 1} };

		Float3 buffer[12];
		Float3* input = verts;
		Float3* output = buffer;
		unsigned int inCount = 4;
		unsigned int outCount = 0;
					
		for(int a = 0; a < 2; ++a)
		{
			int index = clipAxes[nAxis][a];

			Float3 planeN(0.0f, 0.0f, 0.0f);
			planeN.v[index] = 1.0f;
			float planeOffset = aabb.extents.v[index];
			
			clipPolygonToPlaneNegative( planeN, planeOffset, input, inCount, output, outCount );

			inCount = outCount;
			outCount = 0;
			Float3* temp = input;
			input = output;
			output = temp;

			planeN.v[index] = -1.0f;
			clipPolygonToPlaneNegative( planeN, planeOffset, input, inCount, output, outCount );

			inCount = outCount;
			outCount = 0;
			temp = input;
			input = output;
			output = temp;
		}

		if( inCount == 0 )
			return false;

		int size = (int)inCount;
		float distances[12];
		for(int i = 0; i < size; ++i)
		{
			distances[i] = input[i].v[nAxis] * vAxis.v[nAxis];
		}

		for(int k = 0; k < size; ++k)
		{
			for(int i = 0; i < size - 1; ++i)
			{
				if( distances[i] > distances[i+1] )
				{
					swap(distances[i], distances[i+1]);
					swap(input[i], input[i+1]);
				}
			}
		}

		Float3 avg(0.0f, 0.0f, 0.0f);
		unsigned int count = 0;
		for(; count < inCount; ++count)
		{
			if( abs( distances[0] - distances[count] ) > 0.01f )
				break;

			avg += input[count];
		}

		avg *= 1.0f / count;

		contactPt = avg + aabb.center;

		return true;
	}

	bool ObbToObbSolution(const Obb& obb1, const Obb& obb2, Contact* contact)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "OBB Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ObbToObbSolution");
			added = true;
		}

		const Float4x4& xformA = obb1.transform;
		const Float3& extentsA = obb1.extents;

		const Float4x4& xformB = obb2.transform;
		const Float3& extentsB = obb2.extents;

		//float ra;
		//float rb;
		float R[3][3];
		float AbsR[3][3];

		R[0][0] = DotProduct( xformA.XAxis, xformB.XAxis );
		R[0][1] = DotProduct( xformA.XAxis, xformB.YAxis );
		R[0][2] = DotProduct( xformA.XAxis, xformB.ZAxis );

		R[1][0] = DotProduct( xformA.YAxis, xformB.XAxis );
		R[1][1] = DotProduct( xformA.YAxis, xformB.YAxis );
		R[1][2] = DotProduct( xformA.YAxis, xformB.ZAxis );

		R[2][0] = DotProduct( xformA.ZAxis, xformB.XAxis );
		R[2][1] = DotProduct( xformA.ZAxis, xformB.YAxis );
		R[2][2] = DotProduct( xformA.ZAxis, xformB.ZAxis );

		Float3 t = xformB.WAxis - xformA.WAxis;

		t = Float3( DotProduct( t, xformA.XAxis ), DotProduct( t, xformA.YAxis ), DotProduct( t, xformA.ZAxis ) );
	
		for(int i = 0; i < 3; ++i)
			for(int j = 0; j < 3; ++j)
				AbsR[i][j] = abs( R[i][j] ) + ED_EPSILON;

		float penetrationDepth = FLT_MAX;
		Float3 axis(0.0f, 0.0f, 0.0f);
		int worldAxis = 0;

		for(int i = 0; i < 3; ++i)
		{
			float ra = extentsA.v[i];
			float rb = extentsB.v[0] * AbsR[i][0] + extentsB.v[1] * AbsR[i][1] + extentsB.v[2] * AbsR[i][2];

			float d = abs(t.v[i]);

			float pd = (ra + rb) - d;
			
			if( pd <= 0.0f )
				return false;

			if( pd < penetrationDepth )
			{
				penetrationDepth = pd;
				axis.v[0] = axis.v[1] = axis.v[2] = 0.0f;
				axis.v[i] = 1.0f;
				worldAxis = i;
			}
		}

		for(int i = 0; i < 3; ++i)
		{
			float ra = extentsA.v[0] * AbsR[0][i] + extentsA.v[1] * AbsR[1][i] + extentsA.v[2] * AbsR[2][i];
			float rb = extentsB.v[i];

			float d = abs( t.v[0] * R[0][i] + t.v[1] * R[1][i] + t.v[2] * R[2][i] );

			float pd = (ra + rb) - d;

			if( pd <= 0.0f )
				return false;

			if( pd < penetrationDepth )
			{
				penetrationDepth = pd;
				axis.v[0] = R[0][i];
				axis.v[1] = R[1][i];
				axis.v[2] = R[2][i];
				worldAxis = i + 3;
			}
		}

		Float3 axes[9];
		float pds[9] = {0.0f};

		for(int i = 0; i < 3; ++i)
		{
			Float3 axisA(0.0f, 0.0f, 0.0f);
			axisA.v[i] = 1.0f;

			for(int j = 0; j < 3; ++j)
			{
				Float3 axisB( R[0][j], R[1][j], R[2][j] );

				if( axisB.x < 0.0f )
					axisB.x -= ED_EPSILON;
				else
					axisB.x += ED_EPSILON;

				if( axisB.y < 0.0f )
					axisB.y -= ED_EPSILON;
				else
					axisB.y += ED_EPSILON;

				if( axisB.z < 0.0f )
					axisB.z -= ED_EPSILON;
				else
					axisB.z += ED_EPSILON;

				Float3& currentAxis = axes[i*3+j];
				CrossProduct( currentAxis, axisA, axisB );

				float ra = extentsA.x * abs(currentAxis.x) + extentsA.y * abs(currentAxis.y) + extentsA.z * abs(currentAxis.z);

				float rb = extentsB.x * abs( currentAxis.x * R[0][0] + currentAxis.y * R[1][0] + currentAxis.z * R[2][0] ) + 
							extentsB.y * abs( currentAxis.x * R[0][1] + currentAxis.y * R[1][1] + currentAxis.z * R[2][1] ) + 
							extentsB.z * abs( currentAxis.x * R[0][2] + currentAxis.y * R[1][2] + currentAxis.z * R[2][2] );

				float& pd = pds[i*3+j];

				pd = (ra + rb) - abs(DotProduct( t, currentAxis ));

				if( pd <= 0.0f )
					return false;
			}
		}

		if( contact )
		{
			int axisA = -1;
			int axisB = -1;
			for(int i = 0; i < 3; ++i)
			{
				for(int j = 0; j < 3; ++j)
				{
					int index = i*3+j;
					
					Float3& currentAxis = axes[index];
					float& pd = pds[index];

					float mag = currentAxis.magnitude();

					//if( mag < 1.0e-05f )
						//continue;

					float invMag = 1.0f / mag;

					pd *= invMag;

					//if( pd <= 0.01f )
						//return false;
					
					if( pd <= 0.0f )
						return false;

					if( pd < penetrationDepth )
					{
						penetrationDepth = pd;
						axis = currentAxis * invMag;
						axisA = i;
						axisB = j;
					}
				}
			}

			if( DotProduct( axis, t ) > 0.0f )
				axis.negate();
			
			// Edge-Edge
			if( axisA != -1 )
			{
				int worldAxisA = axisA;
				int worldAxisB = axisB;

				Float3 pA, qA, pB, qB;

				Float3 scaledAxisA = Float3( -axis.x / extentsA.x, -axis.y / extentsA.y, -axis.z / extentsA.z );
				int faceAxisA = 0;
				if( abs(scaledAxisA.x) < abs(scaledAxisA.y) )
					faceAxisA = 1;
				if( abs(scaledAxisA.v[faceAxisA]) < abs(scaledAxisA.z) )
					faceAxisA = 2;

				pA.v[worldAxisA] = extentsA.v[worldAxisA];
				qA.v[worldAxisA] = -extentsA.v[worldAxisA];

				pA.v[faceAxisA] = qA.v[faceAxisA] = scaledAxisA.v[faceAxisA] < 0.0f ? -extentsA.v[faceAxisA] : extentsA.v[faceAxisA];

				int thirdAxis = 3 - (worldAxisA | faceAxisA);

				pA.v[thirdAxis] = qA.v[thirdAxis] = scaledAxisA.v[thirdAxis] < 0.0f ? -extentsA.v[thirdAxis] : extentsA.v[thirdAxis];

				Float3 scaledAxisB;
				TransformVector( scaledAxisA, axis, xformA );
				InvTransformVector( scaledAxisB, scaledAxisA, xformB );

				scaledAxisB.x /= extentsB.x;
				scaledAxisB.y /= extentsB.y;
				scaledAxisB.z /= extentsB.z;

				int faceAxisB = 0;
				if( abs(scaledAxisB.x) < abs(scaledAxisB.y) )
					faceAxisB = 1;
				if( abs(scaledAxisB.v[faceAxisB]) < abs(scaledAxisB.z) )
					faceAxisB = 2;

				pB.v[worldAxisB] = extentsB.v[worldAxisB];
				qB.v[worldAxisB] = -extentsB.v[worldAxisB];

				pB.v[faceAxisB] = qB.v[faceAxisB] = scaledAxisB.v[faceAxisB] < 0.0f ? -extentsB.v[faceAxisB] : extentsB.v[faceAxisB];

				thirdAxis = 3 - (worldAxisB | faceAxisB);

				pB.v[thirdAxis] = qB.v[thirdAxis] = scaledAxisB.v[thirdAxis] < 0.0f ? -extentsB.v[thirdAxis] : extentsB.v[thirdAxis];

				TransformPoint( pB, pB, xformB );
				TransformPoint( qB, qB, xformB );
				InvTransformPoint( pB, pB, xformA );
				InvTransformPoint( qB, qB, xformA );

				Float3 cpA;
				Float3 cpB;

				ClosestPtSegmentSegment( pA, qA, pB, qB, cpA, cpB );

				TransformPoint( contact->Point[0], cpA, xformA );
				contact->numPoints = 1;
				TransformVector( contact->Normal, axis, xformA );
				contact->PenetrationDepth = penetrationDepth;
			}
			else //Edge-Face / Face-Face
			{
				if( worldAxis < 3 )
				{
					Obb localObb;
					InvTransformObb(localObb, obb2, obb1.transform);
					Aabb localAabb;
					localAabb.center.makeZero();
					localAabb.extents = obb1.extents;

					Float3 contactPt;
					if( !ObbToAabbFaceContact(localObb, localAabb, worldAxis, contactPt ) )
						return false;

					TransformPoint( contactPt, contactPt, obb1.transform );
					contactPt -= axis * penetrationDepth;

					contact->Point[0] = contactPt;
					contact->numPoints = 1;
					TransformVector( contact->Normal, axis, xformA );
					contact->PenetrationDepth = penetrationDepth;
				}
				else
				{
					worldAxis -= 3;

					Obb localObb;
					InvTransformObb(localObb, obb1, obb2.transform);
					Aabb localAabb;
					localAabb.center.makeZero();
					localAabb.extents = obb2.extents;

					Float3 contactPt;
					if( !ObbToAabbFaceContact(localObb, localAabb, worldAxis, contactPt ) )
						return false;

					TransformPoint( contactPt, contactPt, obb2.transform );

					contact->Point[0] = contactPt;
					contact->numPoints = 1;
					TransformVector( contact->Normal, axis, xformA );
					contact->PenetrationDepth = penetrationDepth;
				}
			}
		}

		return true;
	}

	bool ObbToTriangleSolution(const Obb& obb, const Float3& a, const Float3& b, const Float3& c, Contact* contact)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "OBB Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::ObbToTriangleSolution");
			added = true;
		}

		Float3 v[3];

		InvTransformPoint( v[0], a, obb.transform );
		InvTransformPoint( v[1], b, obb.transform );
		InvTransformPoint( v[2], c, obb.transform );

		float minPen = FLT_MAX;
		Float3 bestAxis(0.0f, 0.0f, 0.0f);
		int axis = 0;

		for(int i = 0; i < 3; ++i)
		{
			float fmin = FLT_MAX;
			float fmax = -FLT_MAX;

			for(int j = 0; j < 3; ++j)
			{
				fmin = min( fmin, v[j].v[i] );
				fmax = max( fmax, v[j].v[i] );
			}

			float fmid = (fmin + fmax) * 0.5f;
			float r = fmax - fmid;

			float pd = r + obb.extents.v[i] - abs(fmid);

			if( pd <= 0.0f )
				return false;

			if( pd < minPen )
			{
				minPen = pd;
				axis = i;
			}
		}

		bestAxis.v[axis] = 1.0f;

		Float3 edges[3] = { v[1] - v[0], v[2] - v[1], v[0] - v[2] };
		Float3 norm;
		CrossProduct( norm, edges[0], edges[1] );

		float r = obb.extents.x * abs(norm.x) + obb.extents.y * abs(norm.y) + obb.extents.z * abs(norm.z);

		float pd = r + DotProduct( norm, v[0] );

		if( pd <= 0.0f )
			return false;

		Float3 axes[9];
		float pds[9];

		// For each box axis...
		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 3; ++j)
			{
				if( edges[i].v[j] < 0.0f )
					edges[i].v[j] -= ED_EPSILON;
				else
					edges[i].v[j] += ED_EPSILON;
			}

			axes[i*3] = Float3(0.0f, -edges[i].z, edges[i].y);
			axes[i*3+1] = Float3(edges[i].z, 0.0f, -edges[i].x);
			axes[i*3+2] = Float3(-edges[i].y, edges[i].x, 0.0f);

			// For each axis...
			for(int j = 0; j < 3; ++j)
			{
				Float3& axis = axes[i*3+j];

				float& pd = pds[i*3+j];

				float fmin = FLT_MAX;
				float fmax = -FLT_MAX;

				for(int k = 0; k < 3; ++k)
				{
					float vertOffset = DotProduct( v[k], axis );
					fmin = min( fmin, vertOffset);
					fmax = max( fmax, vertOffset);
				}

				float fmid = (fmin + fmax) * 0.5f;
				float rt = fmax - fmid;

				float rb = obb.extents.x * abs(axis.x) + obb.extents.y * abs(axis.y) + obb.extents.z * abs(axis.z);

				pd = rt + rb - abs(fmid);

				if( pd <= 0.0f )
					return false;
			}
		}

		if( contact )
		{
			float invMag = 1.0f / norm.magnitude();
			norm *= invMag;
			pd *= invMag;

			if( pd < minPen )
			{
				minPen = pd;
				bestAxis = norm;
			}
		
			for(int i = 0; i < 9; ++i)
			{
				invMag = 1.0f / axes[i].magnitude();
				axes[i] *= invMag;
				pds[i] *= invMag;

				if( pds[i] < minPen )
				{
					minPen = pds[i];
					bestAxis = axes[i];
				}
			}
		
			Float3 tc = (a + b + c) * (1.0f/3.0f);
			Float3 t = obb.transform.WAxis - tc;
		
			EDMath::TransformVector( bestAxis, bestAxis, obb.transform );

			if( DotProduct( bestAxis, t ) < 0.0f )
				bestAxis.negate();

			contact->Normal = bestAxis;
			contact->PenetrationDepth = minPen;
			contact->numPoints = 0;
		}

		return true;
	}

	bool SloppyObbToAabbSolution(const Obb& obb, const Aabb& aabb)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "OBB Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::SloppyObbToAabbSolution");
			added = true;
		}

		Float3 t = obb.transform.WAxis - aabb.center;

		float AbsR[3][3];

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 3; ++j)
				AbsR[j][i] = abs(obb.transform.m[j][i]);

			float r = obb.extents.x * AbsR[0][i] + obb.extents.y * AbsR[1][i] + obb.extents.z * AbsR[2][i];

			if( abs(t.v[i]) >= aabb.extents.v[i] + r )
				return false;
		}	

		for(int i = 0; i < 3; ++i)
		{
			float r = aabb.extents.x * AbsR[i][0] + aabb.extents.y * AbsR[i][1] + aabb.extents.z * AbsR[i][2];

			if( abs( t.v[0] * obb.transform.m[i][0] + t.v[1] * obb.transform.m[i][1] + t.v[2] * obb.transform.m[i][2] ) >= obb.extents.v[i] + r )
				return false;
		}

		return true;
	}

	bool AabbToObbSolution(const Aabb& aabb, const Obb& obb, Contact* contact)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED3", "OBB Collision", Float3(1.0f, 0.0f, 0.0f), 
				L"EDCollision::AabbToObbSolution");
			added = true;
		}

		Obb thisObb;
		thisObb.transform.makeIdentity();
		thisObb.transform.WAxis = aabb.center;
		thisObb.extents = aabb.extents;

		return ObbToObbSolution( thisObb, obb, contact );
	}
}