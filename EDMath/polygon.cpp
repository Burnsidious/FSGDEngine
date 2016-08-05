#include "polygon.h"
#include <algorithm>

#ifndef ED_EPSILON
#define ED_EPSILON 3.45266983072520e-04F
#endif

namespace EDMath
{
	void clipPolygonToPlaneNegative(const Float3& planeN, float planeOffset, const Float3* verts, unsigned int vertCount, Float3* out, unsigned int& outCount)
	{
		const float CLIP_EPSILON = FLT_EPSILON;
		const unsigned int START_IN = 0x0;
		const unsigned int START_ON = 0x1;
		const unsigned int START_OUT = 0x2;

		const unsigned int END_IN = 0x0;
		const unsigned int END_ON = 0x4;
		const unsigned int END_OUT = 0x8;

		outCount = 0;

		switch( std::min( vertCount, 3U ) )
		{
		case 0:
			return;
		case 1:
			{
				float offset = DotProduct( verts[0], planeN ) - planeOffset;
				if( offset <= CLIP_EPSILON )
				{
					out[0] = verts[0];
					outCount = 1;
				}
			}
			break;
		case 2:
			{
				const Float3& start = verts[0];
				const Float3& end = verts[1];
			
				float startOffset = DotProduct( start, planeN ) - planeOffset;
				float endOffset = DotProduct( end, planeN ) - planeOffset;

				unsigned int result = START_IN | END_IN;

				if( abs(startOffset) <= CLIP_EPSILON )
					result |= START_ON;
				else if( startOffset > CLIP_EPSILON )
					result |= START_OUT;

				if( abs(endOffset) <= CLIP_EPSILON )
					result |= END_ON;
				else if( endOffset > CLIP_EPSILON )
					result |= END_OUT;

				switch( result )
				{
				case ( START_IN | END_OUT ):
					{
						Float3 edge = end - start;
						float d = -startOffset / DotProduct( edge, planeN );

						out[0] = start;
						out[1] = start + edge * d;
						outCount = 2;					
					}
					break;
				case ( START_OUT | END_IN ):
					{
						Float3 edge = end - start;
						float d = -startOffset / DotProduct( edge, planeN );

						out[0] = start + edge * d;
						out[1] = end;
						outCount = 2;
					}
				case ( START_OUT | END_OUT ):
					break;
				case ( START_ON | END_OUT ):
					{
						out[0] = start;
						outCount = 1;
					}
					break;
				case ( START_OUT | END_ON ):
					{
						out[0] = end;
						outCount = 1;
					}
					break;
				default:
					{
						out[0] = start;
						out[1] = end;
						outCount = 2;
					}
					break;
				}		
			}
			break;
		default:
			{
				Float3 start = verts[vertCount-1];

				for(unsigned int i = 0; i < vertCount; ++i)
				{
					const Float3& end = verts[i];

					float startOffset = DotProduct( start, planeN ) - planeOffset;	
					float endOffset = DotProduct( end, planeN ) - planeOffset;

					unsigned int result = START_IN | END_IN;

					if( abs(startOffset) <= CLIP_EPSILON )
						result |= START_ON;
					else if( startOffset > CLIP_EPSILON )
						result |= START_OUT;

					if( abs(endOffset) <= CLIP_EPSILON )
						result |= END_ON;
					else if( endOffset > CLIP_EPSILON )
						result |= END_OUT;

					switch( result )
					{
					case ( START_IN | END_OUT ):
						{
							Float3 edge = end - start;
							float d = -startOffset / DotProduct( edge, planeN );

							out[outCount] = start + edge * d;
							++outCount;					
						}
						break;
					case ( START_OUT | END_IN ):
						{
							Float3 edge = end - start;
							float d = -startOffset / DotProduct( edge, planeN );

							out[outCount] = start + edge * d;
							++outCount;	

							out[outCount] = end;
							++outCount;
						}
					case ( START_OUT | END_OUT ):
					case ( START_ON | END_OUT ):
						break;
					default:
						{
							out[outCount] = end;
							++outCount;
						}
						break;
					}

					start = end;
				}
			}
			break;
		}
	}
}