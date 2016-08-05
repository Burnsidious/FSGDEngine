#pragma once
#include <math.h>
#include <utility>

namespace EDMath
{
    // A simple class used to define a 3d floating point vector
    // with common functionality
    class Float3
    {
    public:

        // Used to allow the data in the vector to 
        // be accessed in different ways
        union
        {
            // Access the vector as an array
            float v[3];

            // Access the vector's individual components
            struct
            {
                float x;
                float y;
                float z;
            };
			struct 
			{
                float r;
                float g;
                float b;
			};
        };

        // Constructors
        Float3() { x = y = z = 0; }
        Float3(float _x, float _y, float _z)
        { x = _x; y = _y; z = _z; }

		Float3(const Float3 & rhs)
		{
			*this = rhs;
		}

		Float3(const Float3 && rhs)
		{
			*this = std::move(rhs);
		}

        inline Float3 operator+(const Float3 &v) const
        {
            return Float3(x + v.x, y + v.y, z + v.z);
        }
        inline Float3 operator-(const Float3 &v) const
        {
            return Float3(x - v.x, y - v.y, z - v.z);
        }
        inline Float3 operator*(const Float3 &v) const
        {
            return Float3(x * v.x, y * v.y, z * v.z);
        }
        inline Float3 operator/(const Float3 &v) const
        {
            return Float3(x / v.x, y / v.y, z / v.z);
        }
        inline Float3 operator*(float v) const
        {
            return Float3(x * v, y * v, z * v);
        }
        inline Float3 operator/(float v) const
        {
            return Float3(x / v, y / v, z / v);
        }

        inline Float3 &operator=(const Float3 &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }
		inline Float3 &operator=(const Float3&& v)
        {
			if(this != &v)
			{
				x = std::move(v.x);
				y = std::move(v.y);
				z = std::move(v.z);
			}
            return *this;
        }
        inline Float3 &operator+=(const Float3 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        inline Float3 &operator-=(const Float3 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        inline Float3 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }

        inline Float3 &makeZero()
        {
            x = y = z = 0;
            return *this;
        }

        inline Float3 &negate()
        {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }

        inline float dotProduct(Float3 v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }

        inline friend float DotProduct(Float3 a, Float3 b)
        {
            return a.dotProduct(b);
        }

        inline friend void CrossProduct(Float3 &out, Float3 a, Float3 b)
        {
            out.x = a.y * b.z - a.z * b.y;
            out.y = a.z * b.x - a.x * b.z;
            out.z = a.x * b.y - a.y * b.x;
        }

        inline float magnitude() const
        {
            return (float)sqrt(dotProduct(*this));
        }

		inline float squaredMagnitude() const
		{
			return dotProduct(*this);
		}

        inline Float3 &normalize()
        {
            float invMag = 1 / magnitude();
            *this *= invMag;
            return *this;
        }

        //operator D3DXVECTOR3 *() { return (D3DXVECTOR3 *)this; }

    };

	extern const Float3 ZERO_VECTOR;
}