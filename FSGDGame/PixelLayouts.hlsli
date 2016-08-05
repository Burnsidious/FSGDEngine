#ifndef PIXEL_LAYOUTS_HLSLI
#define PIXEL_LAYOUTS_HLSLI

struct ForwardColorOut
{
	float4 diffuse : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 depth : SV_TARGET2;
};

#endif //PIXEL_LAYOUTS_HLSLI
