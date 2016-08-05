#pragma once

#include "Float3.h"

namespace EDMath
{
	// A class used to define a 4 by 4 floating point 
	// matrix with common functionality
	class Float4x4
	{
	public:

		// Used to allow the data in the matrix to 
		// be accessed in different ways
		union
		{
			// The matrix as a 2d array
			float m[4][4];

			// The matrix as a 1d array
			float ma[16];

			// The matrix as individual components
			// Think of them as axis'
			// row vs collum major does not matter
			struct 
			{
				// The X axis
				float Xx, Xy, Xz, Xw;
				// The Y axis
				float Yx, Yy, Yz, Yw;
				// The Z axis
				float Zx, Zy, Zz, Zw;            
				// The W axis - Translation
				float Wx, Wy, Wz, Ww;        
			};

			struct
			{
				// The X axis
				Float3 XAxis;            float padX;
				// The Y axis
				Float3 YAxis;            float padY;
				// The Z axis
				Float3 ZAxis;            float padZ;
				// The W axis - Translation
				Float3 WAxis;            float padW;
			};
		};

		// Constructors
		Float4x4() {}
		Float4x4(   float xx, float xy, float xz, float xw,
			float yx, float yy, float yz, float yw,
			float zx, float zy, float zz, float zw,
			float wx, float wy, float wz, float ww);

		Float4x4(Float4x4 &&rhs) 
		{
			if(this != &rhs)
			{
				*this = std::move(rhs);
			}
		}

		Float4x4 &operator=(Float4x4 &&rhs)
		{
			if(this != &rhs)
			{
				XAxis = std::move(rhs.XAxis);
				YAxis = std::move(rhs.YAxis);
				ZAxis = std::move(rhs.ZAxis);
				WAxis = std::move(rhs.WAxis);

				padX = std::move(rhs.padX);
				padY = std::move(rhs.padY);
				padZ = std::move(rhs.padZ);
				padW = std::move(rhs.padW);
			}
			return *this;
		}

		bool operator==(const Float4x4 &rightMatrix) const 
		{
			return !memcmp(ma, rightMatrix.ma, 16 * sizeof(float));
		}

		// These methods change the matrix into a specific type of matrix
		Float4x4 &makeZero();        
		Float4x4 &makeIdentity();        
		// Changes the matrix to be an X rotation matrix
		Float4x4 &makeRotationX(float radians);
		// Changes the matrix to be an Y rotation matrix
		Float4x4 &makeRotationY(float radians);
		// Changes the matrix to be an Z rotation matrix
		Float4x4 &makeRotationZ(float radians);
		// Changes the matrix to be a scale matrix
		Float4x4 &makeScale(float xScale, float yScale, float zScale);
		// Changes the matrix to be a perspective matrix
		Float4x4 &makePerspectiveLH(float fFov, float fAspect, 
			float fNear, float fFar);

		// Matrix multiply method
		// out = a * b
		friend void Multiply(Float4x4 &out, const Float4x4 &a, const Float4x4 &b);

		friend Float4x4 operator*(const Float4x4 &a, const Float4x4 &b);

		// Matrix multiply method, slower than friend func Multiply
		// temp = this;
		// this = temp * a
		//Float4x4 &operator*=(const Float4x4 &a);

		// Local rotation methods function as pre multiplies
		// this = this * makeRotation
		Float4x4 &rotateLocalX(float radians);
		Float4x4 &rotateLocalY(float radians);
		Float4x4 &rotateLocalZ(float radians);

		// Global rotation methods function as post multiplies
		// this = makeRotation * this
		Float4x4 &rotateGlobalX(float radians);
		Float4x4 &rotateGlobalY(float radians);
		Float4x4 &rotateGlobalZ(float radians);

		Float4x4 &translateLocal(const Float3 &tran)
		{
			return translateLocal(tran.x, tran.y, tran.z);
		}
		Float4x4 &translateLocal(float x, float y, float z);

		Float4x4 &translateGlobal(const Float3 &tran)
		{
			return translateGlobal(tran.x, tran.y, tran.z);
		}
		Float4x4 &translateGlobal(float x, float y, float z);

		// Scales the x, y and z axis' of the matrix
		Float4x4 &scale(const Float3 &scaleVec);
		// Scales the x, y and z axis' of the matrix
		Float4x4 &scale(float x, float y, float z);
		// Scales the x, y, and z axes of the matrix
		Float4x4 &scale(float scalar);

		// Transposes the matrix
		Float4x4 &transpose(void);
		// Transposes the orientation of the matrix
		Float4x4 &transpose3x3(void);

		friend void TransformVector(Float3 &out, const Float3 &inVec,
			const Float4x4 &inMat);
		friend void TransformPoint(Float3 &out, const Float3 &inVec,
			const Float4x4 &inMat);

		friend void InvTransformVector(Float3 &out, const Float3 &inVec,
			const Float4x4 &inMat);
		friend void InvTransformPoint(Float3 &out, const Float3 &inVec,
			const Float4x4 &inMat);

		friend Float4x4 Interpolate(const Float4x4 &matA, const Float4x4 &matB, float delta);

		Float3 getScale(void) const;

		//operator const D3DXMATRIX *() const { return (const D3DXMATRIX *)this; }

		void TransformVector(Float3 &vector);
		void TransformPoint(Float3 &vector);

		void InvTransformVector(Float3 &vector);
		void InvTransformPoint(Float3 &vector);

		// Returns true if the difference between each component of this matrix 
		// and otherMat are within the threshold defined be threshold
		bool CompareComponents(const Float4x4 &otherMat, float threshold) const;
	};

	extern const Float4x4 IDENTITY;
}
