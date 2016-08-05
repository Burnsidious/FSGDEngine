#include "Float4x4.h"
#include <algorithm>

namespace EDMath
{
	const Float4x4 IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f);

    Float4x4::Float4x4( float xx, float xy, float xz, float xw,
        float yx, float yy, float yz, float yw,
        float zx, float zy, float zz, float zw,
        float wx, float wy, float wz, float ww)
    {
        Xx = xx; Xy = xy; Xz = xz; Xw = xw;
        Yx = yx; Yy = yy; Yz = yz; Yw = yw;
        Zx = zx; Zy = zy; Zz = zz; Zw = zw;
        Wx = wx; Wy = wy; Wz = wz; Ww = ww;
    }

    Float4x4 &Float4x4::makeZero()
    {
        Xx = Xy = Xz = Xw = 
            Yx = Yy = Yz = Yw = 
            Zx = Zy = Zz = Zw = 
            Wx = Wy = Wz = Ww = 0;    
        return *this;
    }

    Float4x4 &Float4x4::makeIdentity()
    {
        Xy = Xz = Xw = 
            Yx = Yz = Yw = 
            Zx = Zy = Zw = 
            Wx = Wy = Wz = 0;  

        Xx = Yy = Zz = Ww = 1;
        return *this;
    }
    Float4x4 &Float4x4::makeRotationX(float radians)
    {
        float sinAngle = sin(radians);
        float cosAngle = cos(radians);

        Xx = 1;     Xy = 0;         Xz = 0;         Xw = 0;
        Yx = 0;     Yy = cosAngle;  Yz = sinAngle;  Yw = 0;
        Zx = 0;     Zy = -sinAngle; Zz = cosAngle;  Zw = 0;
        Wx = 0;     Wy = 0;         Wz = 0;         Ww = 1;

        return *this;
    }

    Float4x4 &Float4x4::makeRotationY(float radians)
    {
        float sinAngle = sin(radians);
        float cosAngle = cos(radians);

        Xx = cosAngle;  Xy = 0; Xz = -sinAngle; Xw = 0;
        Yx = 0;         Yy = 1; Yz = 0;         Yw = 0;
        Zx = sinAngle;  Zy = 0; Zz = cosAngle;  Zw = 0;
        Wx = 0;         Wy = 0; Wz = 0;         Ww = 1;
        return *this;
    }

    Float4x4 &Float4x4::makeRotationZ(float radians)
    {
        float sinAngle = sin(radians);
        float cosAngle = cos(radians);

        Xx = cosAngle;  Xy = sinAngle;  Xz = 0; Xw = 0;
        Yx = -sinAngle; Yy = cosAngle;  Yz = 0; Yw = 0;
        Zx = 0;         Zy = 0;         Zz = 1; Zw = 0;
        Wx = 0;         Wy = 0;         Wz = 0; Ww = 1;
        return *this;
    }

    Float4x4 &Float4x4::makeScale(float xScale, float yScale, float zScale)
    {
        Xy = Xz = Xw = 
            Yx = Yz = Yw = 
            Zx = Zy = Zw = 
            Wx = Wy = Wz = 0;  

        Xx = xScale;
        Yy = yScale;
        Zz = zScale;
        Ww = 1;
        return *this;
    }

    Float4x4 &Float4x4::makePerspectiveLH(float fFov, float fAspect, 
        float fNear, float fFar)
    {
		float yScale = 1.0f / tan(fFov/2);
		float xScale = yScale / fAspect;

        makeIdentity();
		m[0][0] = xScale;
		m[1][1] = yScale;
		m[2][2] = fFar/(fFar-fNear);
		m[3][2] = -fNear * fFar/(fFar-fNear);
		m[2][3] = 1;
		m[3][3] = 0;
        return *this;

        //float xmin, xmax, ymin, ymax;       // Dimensions of near clipping plane

        //// Do the Math for the near clipping plane
        //ymax = fNear * float(tan( fFov * 3.14159265358979323846 / 360.0 ));
        //ymin = -ymax;
        //xmin = ymin * fAspect;
        //xmax = -xmin;

        //makeIdentity();
        //// Construct the projection matrix
        //ma[0] = (2.0f * fNear)/(xmax - xmin);
        //ma[5] = (2.0f * fNear)/(ymax - ymin);
        //ma[8] = (xmax + xmin) / (xmax - xmin);
        //ma[9] = (ymax + ymin) / (ymax - ymin);
        //ma[10] = -((fFar + fNear)/(fFar - fNear));
        //ma[11] = -1.0f;
        //ma[14] = -((2.0f * fFar * fNear)/(fFar - fNear));
        //ma[15] = 0.0f;
        //return *this;
    }

    void Multiply(Float4x4 &out, const Float4x4 &a, const Float4x4 &b)
    {
		out = Float4x4(
			    a.Xx * b.Xx + a.Xy * b.Yx + a.Xz * b.Zx + a.Xw * b.Wx,
				a.Xx * b.Xy + a.Xy * b.Yy + a.Xz * b.Zy + a.Xw * b.Wy,
				a.Xx * b.Xz + a.Xy * b.Yz + a.Xz * b.Zz + a.Xw * b.Wz,
				a.Xx * b.Xw + a.Xy * b.Yw + a.Xz * b.Zw + a.Xw * b.Ww,

				a.Yx * b.Xx + a.Yy * b.Yx + a.Yz * b.Zx + a.Yw * b.Wx,
				a.Yx * b.Xy + a.Yy * b.Yy + a.Yz * b.Zy + a.Yw * b.Wy,
				a.Yx * b.Xz + a.Yy * b.Yz + a.Yz * b.Zz + a.Yw * b.Wz,
				a.Yx * b.Xw + a.Yy * b.Yw + a.Yz * b.Zw + a.Yw * b.Ww,

				a.Zx * b.Xx + a.Zy * b.Yx + a.Zz * b.Zx + a.Zw * b.Wx,
				a.Zx * b.Xy + a.Zy * b.Yy + a.Zz * b.Zy + a.Zw * b.Wy,
				a.Zx * b.Xz + a.Zy * b.Yz + a.Zz * b.Zz + a.Zw * b.Wz,
				a.Zx * b.Xw + a.Zy * b.Yw + a.Zz * b.Zw + a.Zw * b.Ww,

				a.Wx * b.Xx + a.Wy * b.Yx + a.Wz * b.Zx + a.Ww * b.Wx,
				a.Wx * b.Xy + a.Wy * b.Yy + a.Wz * b.Zy + a.Ww * b.Wy,
				a.Wx * b.Xz + a.Wy * b.Yz + a.Wz * b.Zz + a.Ww * b.Wz,
				a.Wx * b.Xw + a.Wy * b.Yw + a.Wz * b.Zw + a.Ww * b.Ww);
    }

	Float4x4 operator*(const Float4x4 &a, const Float4x4 &b)
	{
		return Float4x4(
			    a.Xx * b.Xx + a.Xy * b.Yx + a.Xz * b.Zx + a.Xw * b.Wx,
				a.Xx * b.Xy + a.Xy * b.Yy + a.Xz * b.Zy + a.Xw * b.Wy,
				a.Xx * b.Xz + a.Xy * b.Yz + a.Xz * b.Zz + a.Xw * b.Wz,
				a.Xx * b.Xw + a.Xy * b.Yw + a.Xz * b.Zw + a.Xw * b.Ww,

				a.Yx * b.Xx + a.Yy * b.Yx + a.Yz * b.Zx + a.Yw * b.Wx,
				a.Yx * b.Xy + a.Yy * b.Yy + a.Yz * b.Zy + a.Yw * b.Wy,
				a.Yx * b.Xz + a.Yy * b.Yz + a.Yz * b.Zz + a.Yw * b.Wz,
				a.Yx * b.Xw + a.Yy * b.Yw + a.Yz * b.Zw + a.Yw * b.Ww,

				a.Zx * b.Xx + a.Zy * b.Yx + a.Zz * b.Zx + a.Zw * b.Wx,
				a.Zx * b.Xy + a.Zy * b.Yy + a.Zz * b.Zy + a.Zw * b.Wy,
				a.Zx * b.Xz + a.Zy * b.Yz + a.Zz * b.Zz + a.Zw * b.Wz,
				a.Zx * b.Xw + a.Zy * b.Yw + a.Zz * b.Zw + a.Zw * b.Ww,

				a.Wx * b.Xx + a.Wy * b.Yx + a.Wz * b.Zx + a.Ww * b.Wx,
				a.Wx * b.Xy + a.Wy * b.Yy + a.Wz * b.Zy + a.Ww * b.Wy,
				a.Wx * b.Xz + a.Wy * b.Yz + a.Wz * b.Zz + a.Ww * b.Wz,
				a.Wx * b.Xw + a.Wy * b.Yw + a.Wz * b.Zw + a.Ww * b.Ww);
	}

    //Float4x4 &Float4x4::operator*=(const Float4x4 &a)
    //{
    //    Float4x4 temp = *this;
    //    Multiply(*this, temp, a);
    //    return *this;
    //}

    Float4x4 &Float4x4::rotateLocalX(float radians)
    {
        Float4x4 temp = *this;
        Multiply(*this, Float4x4().makeRotationX(radians), temp);
        return *this;
    }

    Float4x4 &Float4x4::rotateLocalY(float radians)
    {
        Float4x4 temp = *this;
        Multiply(*this, Float4x4().makeRotationY(radians), temp);
        return *this;
    }

    Float4x4 &Float4x4::rotateLocalZ(float radians)
    {
        Float4x4 temp = *this;
        Multiply(*this, Float4x4().makeRotationZ(radians), temp);
        return *this;
    }

    Float4x4 &Float4x4::rotateGlobalX(float radians)
    {
        Float4x4 temp = *this;
        Multiply(*this, temp, Float4x4().makeRotationX(radians));
        return *this;
    }

    Float4x4 &Float4x4::rotateGlobalY(float radians)
    {
        Float4x4 temp = *this;
        Multiply(*this, temp, Float4x4().makeRotationY(radians));
        return *this;
    }

    Float4x4 &Float4x4::rotateGlobalZ(float radians)
    {
        Float4x4 temp = *this;
        Multiply(*this, temp, Float4x4().makeRotationZ(radians));
        return *this;
    }

    Float4x4 &Float4x4::translateLocal(float x, float y, float z)
    {
        WAxis +=
            XAxis * x +
            YAxis * y +
            ZAxis * z;
        return *this;
    }

    Float4x4 &Float4x4::translateGlobal(float x, float y, float z)
    {
        WAxis.x += x;
        WAxis.y += y;
        WAxis.z += z;
        return *this;
    }

    Float4x4 &Float4x4::scale(const Float3 &scaleVec)
    {
        return scale(scaleVec.x, scaleVec.y, scaleVec.z);
    }

    Float4x4 &Float4x4::scale(float x, float y, float z)
    {
        XAxis *= x;
        YAxis *= y;
        ZAxis *= z;
        return *this;
    }

    Float4x4 &Float4x4::scale(float scalar)
    {
        XAxis *= scalar;
        YAxis *= scalar;
        ZAxis *= scalar;
        return *this;
    }

    Float4x4 &Float4x4::transpose(void)
    {
        std::swap(Xy, Yx);
        std::swap(Xz, Zx);
        std::swap(Yz, Zy);

        std::swap(Wx, Xw);
        std::swap(Wy, Yw);
        std::swap(Wz, Zw);
        return *this;
    }

    Float4x4 &Float4x4::transpose3x3(void)
    {
        std::swap(Xy, Yx);
        std::swap(Xz, Zx);
        std::swap(Yz, Zy);
        return *this;
    }

    void TransformVector(Float3 &out, const Float3 &inVec,
        const Float4x4 &inMat)
    {
		Float3 vectorIn = inVec;

        out.x = inMat.Xx * vectorIn.x + inMat.Yx * vectorIn.y + 
            inMat.Zx * vectorIn.z;
        out.y = inMat.Xy * vectorIn.x + inMat.Yy * vectorIn.y + 
            inMat.Zy * vectorIn.z;
        out.z = inMat.Xz * vectorIn.x + inMat.Yz * vectorIn.y + 
            inMat.Zz * vectorIn.z;
    }

    void TransformPoint(Float3 &out, const Float3 &inVec,
        const Float4x4 &inMat)
    {
		Float3 vectorIn = inVec;

		out.x = inMat.Xx * vectorIn.x + inMat.Yx * vectorIn.y + 
            inMat.Zx * vectorIn.z + inMat.Wx;
        out.y = inMat.Xy * vectorIn.x + inMat.Yy * vectorIn.y + 
            inMat.Zy * vectorIn.z + inMat.Wy;
        out.z = inMat.Xz * vectorIn.x + inMat.Yz * vectorIn.y + 
            inMat.Zz * vectorIn.z + inMat.Wz;
    }

    void InvTransformVector(Float3 &out, const Float3 &inVec,
        const Float4x4 &inMat)
    {
		out = Float3(	DotProduct( inVec, inMat.XAxis ),
					DotProduct( inVec, inMat.YAxis ),
					DotProduct( inVec, inMat.ZAxis ) );
	}

    void InvTransformPoint(Float3 &out, const Float3 &inVec,
        const Float4x4 &inMat)
    {
		InvTransformVector( out, inVec - inMat.WAxis, inMat );
    }

	Float4x4 Interpolate(const Float4x4 &matA, const Float4x4 &matB, float delta)
	{
		Float4x4 iMat(matA);
		//result = matB;

		// Inverse of MatrixA
		//EDMath::Float4x4 mat = iMat;

		EDMath::Float3 f3X = iMat.XAxis;
		EDMath::Float3 f3Y = iMat.YAxis;
		EDMath::Float3 f3Z = iMat.ZAxis;
		EDMath::Float3 f3W = iMat.WAxis;

		// new W axis is the negative dot of the old W axis with X,Y,Z
		iMat.Wx = -f3X.dotProduct(f3W);
		iMat.Wy = -f3Y.dotProduct(f3W);
		iMat.Wz = -f3Z.dotProduct(f3W);

		// transpose x,y,z axis
		iMat.transpose3x3();

		// Remove MatrixA's transformation from MatrixB
		Multiply(iMat, iMat, matB);

		// iMat is now the intermediary transformation from MatrixA to MatrixB
		// ie: iMat * MatrixA = MatrixB


		// The trace of our matrix
		float trace = 1.0f + iMat.ma[0] + iMat.ma[5] + iMat.ma[10];

		float quatResult[4];

		// Calculate the quaternion of iMat
		// If trace is greater than 0, but consider small values that
		// might result in 0 when operated upon due to floating point error
		if( trace > 0.00000001f )
		{
			float S = sqrt(trace)*2;
			quatResult[0] = (iMat.ma[9] - iMat.ma[6]) / S;
			quatResult[1] = (iMat.ma[2] - iMat.ma[8]) / S;
			quatResult[2] = (iMat.ma[4] - iMat.ma[1]) / S;
			quatResult[3] = 0.25f * S;
		}
		else
		{
			if( iMat.ma[0] > iMat.ma[5] && iMat.ma[0] > iMat.ma[10] )
			{
				float S = sqrt( 1.0f + iMat.ma[0] - iMat.ma[5] - iMat.ma[10] ) * 2;
				quatResult[0] = 0.25f * S;
				quatResult[1] = (iMat.ma[4] + iMat.ma[1]) / S;
				quatResult[2] = (iMat.ma[2] + iMat.ma[8]) / S;
				quatResult[3] = (iMat.ma[9] - iMat.ma[6]) / S;
			}
			else if( iMat.ma[5] > iMat.ma[10] )
			{
				float S = sqrt( 1.0f + iMat.ma[5] - iMat.ma[0] - iMat.ma[10] ) * 2;
				quatResult[0] = (iMat.ma[4] + iMat.ma[1]) / S;
				quatResult[1] = 0.25f * S;
				quatResult[2] = (iMat.ma[9] + iMat.ma[6]) / S;
				quatResult[3] = (iMat.ma[2] - iMat.ma[8]) / S;
			}
			else
			{
				float S = sqrt( 1.0f + iMat.ma[10] - iMat.ma[0] - iMat.ma[5] ) * 2;
				quatResult[0] = (iMat.ma[2] + iMat.ma[8]) / S;
				quatResult[1] = (iMat.ma[9] + iMat.ma[6]) / S;
				quatResult[2] = 0.25f * S;
				quatResult[3] = (iMat.ma[4] - iMat.ma[1]) / S;
			}
		}

		// Get the magnitude of our quaternion
		float quatMagnitude = sqrt( quatResult[0]*quatResult[0] + quatResult[1]*quatResult[1] + quatResult[2]*quatResult[2] + quatResult[3]*quatResult[3] );

		// Normalize our quaternion
		float invQuatMag = 1.0f/quatMagnitude;
		float quatNormalized[4] = { quatResult[0]*invQuatMag, quatResult[1]*invQuatMag, quatResult[2]*invQuatMag, quatResult[3]*invQuatMag }; 

		// Calculate the angles relevant to our quaternion
		float cos_a = quatNormalized[3];
		float angle = acos( cos_a ) * 2;
		float sin_a = sqrt( 1.0f - cos_a * cos_a );

		// If there was no rotation between matrices, calculation
		// of the rotation matrix will end badly. So just do the linear
		// interpolation of the translation component and return
		if( angle == 0.0f )
		{
			return Float4x4(
				matA.Xx, matA.Xy, matA.Xz, matA.Xw,
				matA.Yx, matA.Yy, matA.Yz, matA.Yw,
				matA.Zx, matA.Zy, matA.Zz, matA.Zw,

				matA.m[3][0] + ((matB.m[3][0]-matA.m[3][0])*delta),
				matA.m[3][1] + ((matB.m[3][1]-matA.m[3][1])*delta),
				matA.m[3][2] + ((matB.m[3][2]-matA.m[3][2])*delta),
				matA.Ww);
		}


		// Our axis of abitrary rotation
		Float3 axis;

		if( fabs( sin_a ) < 0.0005f )
			sin_a = 1;

		axis.x = quatNormalized[0] / sin_a;
		axis.y = quatNormalized[1] / sin_a;
		axis.z = quatNormalized[2] / sin_a;

		// Get the portion of the angle to rotate by
		angle *= delta;

		axis.normalize();

		// Calculate the quaternion for our new (partial) angle of rotation
		sin_a = sin( angle / 2 );
		cos_a = cos( angle / 2 );
		quatNormalized[0] = axis.x * sin_a;
		quatNormalized[1] = axis.y * sin_a;
		quatNormalized[2] = axis.z * sin_a;
		quatNormalized[3] = cos_a;

		quatMagnitude = sqrt( quatNormalized[0]*quatNormalized[0] + quatNormalized[1]*quatNormalized[1] + quatNormalized[2]*quatNormalized[2] + quatNormalized[3]*quatNormalized[3] );		
		quatNormalized[0] /= quatMagnitude;
		quatNormalized[1] /= quatMagnitude;
		quatNormalized[2] /= quatMagnitude;
		quatNormalized[3] /= quatMagnitude;

		// Calculate our partial rotation matrix
		float xx      = quatNormalized[0] * quatNormalized[0];
		float xy      = quatNormalized[0] * quatNormalized[1];
		float xz      = quatNormalized[0] * quatNormalized[2];
		float xw      = quatNormalized[0] * quatNormalized[3];
		float yy      = quatNormalized[1] * quatNormalized[1];
		float yz      = quatNormalized[1] * quatNormalized[2];
		float yw      = quatNormalized[1] * quatNormalized[3];
		float zz      = quatNormalized[2] * quatNormalized[2];
		float zw      = quatNormalized[2] * quatNormalized[3];

		Float4x4 result;
		result.makeIdentity();

		result.ma[0]  = 1 - 2 * ( yy + zz );
		result.ma[1]  =     2 * ( xy - zw );
		result.ma[2]  =     2 * ( xz + yw );
		result.ma[4]  =     2 * ( xy + zw );
		result.ma[5]  = 1 - 2 * ( xx + zz );
		result.ma[6]  =     2 * ( yz - xw );
		result.ma[8]  =     2 * ( xz - yw );
		result.ma[9]  =     2 * ( yz + xw );
		result.ma[10] = 1 - 2 * ( xx + yy );
		result.ma[3]  = result.ma[7] = result.ma[11] = result.ma[12] = result.ma[13] = result.ma[14] = 0;
		result.ma[15] = 1;

		// Combine our partial rotation with MatrixA
		Multiply(result, matA, result);

		// Linear interpolation of the translation components of the matrices
		result.m[3][0] = matA.m[3][0] + ((matB.m[3][0]-matA.m[3][0])*delta);
		result.m[3][1] = matA.m[3][1] + ((matB.m[3][1]-matA.m[3][1])*delta);
		result.m[3][2] = matA.m[3][2] + ((matB.m[3][2]-matA.m[3][2])*delta);

		return result;
	}



    Float3 Float4x4::getScale(void) const
    {
        return Float3(XAxis.magnitude(), YAxis.magnitude(), ZAxis.magnitude());
    }

	void Float4x4::TransformVector(Float3 &vector)
	{
		Float3 vectorIn = vector;
		vector.x = Xx * vectorIn.x + Yx * vectorIn.y + 
			Zx * vectorIn.z;
		vector.y = Xy * vectorIn.x + Yy * vectorIn.y + 
			Zy * vectorIn.z;
		vector.z = Xz * vectorIn.x + Yz * vectorIn.y + 
			Zz * vectorIn.z;
	}

	void Float4x4::TransformPoint(Float3 &vector)
	{
		Float3 vectorIn = vector;
		vector.x = Xx * vectorIn.x + Yx * vectorIn.y + 
			Zx * vectorIn.z + Wx;
		vector.y = Xy * vectorIn.x + Yy * vectorIn.y + 
			Zy * vectorIn.z + Wy;
		vector.z = Xz * vectorIn.x + Yz * vectorIn.y + 
			Zz * vectorIn.z + Wz;
	}

	void Float4x4::InvTransformVector(Float3 &vector)
	{
		vector = Float3( DotProduct( vector, XAxis ),
					DotProduct( vector, YAxis ),
					DotProduct( vector, ZAxis ) );
	}

	void Float4x4::InvTransformPoint(Float3 &vector)
	{
		vector -= WAxis;
		InvTransformVector( vector );
	}

	bool Float4x4::CompareComponents(const Float4x4 &otherMat, float threshold) const 
	{
		for(unsigned int i = 0; i < 16; ++i)
		{
			if(abs(ma[i] - otherMat.ma[i]) > threshold)
				return false;
		}
		return true;
	}

}
