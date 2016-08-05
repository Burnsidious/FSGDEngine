#include "DepthPoint_VS.hlsli"

DepthVertOut main( VERTEX_POS input )
{
	return DepthPoint(input, 2);
}