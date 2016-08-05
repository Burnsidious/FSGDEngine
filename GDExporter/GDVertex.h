#pragma once
#include "GDAttribute.h"

struct GDVertexAttribute
{
	union
	{
		float m_v[4];

		int joints[4];

		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		struct
		{
			float s;
			float t;
			float q;
			float r;
		};

		struct
		{
			float red;
			float green;
			float blue;
			float alpha;
		};

		struct
		{
			int joint0;
			int joint1;
			int joint2;
			int joint3;
		};

		struct
		{
			float weight0;
			float weight1;
			float weight2;
			float weight3;
		};
	};

	GDVertexAttribute(void) : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

	bool operator==(const GDVertexAttribute& rhs)
	{
		for(unsigned int i = 0; i < 4; ++i)
			if( abs( m_v[i] - rhs.m_v[i] ) > 0.001f )
				return false; 

		return true;
	}

	bool operator!=(const GDVertexAttribute& rhs)
	{
		return !(operator==(rhs));
	}
};

struct GDVertex
{
	GDVertexAttribute attributes[GDVAT_MAX];

	bool operator==(const GDVertex& rhs)
	{
		for(unsigned int i = 0; i < GDVAT_MAX; ++i)
			if( attributes[i] != rhs.attributes[i] )
				return false;

		return true;
	}
};