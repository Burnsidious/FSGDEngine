#ifndef CONSTANT_BUFFERS_HLSLI
#define CONSTANT_BUFFERS_HLSLI
//
//#pragma pack_matrix(row_major)
//
//// Starting with register 3 as the first three buffers are used 
//// for directionalights, point lights and spot lights.
////cbuffer cbPerObject : register(b3)
////{
////	/// The concatenated world view projection matrices
////	float4x4	gMVP;
////	/// The world matrix of the object to be rendered.
////	float4x4	gWorld;
////}
//
//cbuffer cbPerCamera : register(b4)
//{
//	float4x4	gView;
//	float4x4	gInvViewProj;
//	/// The position of the camera, or eye.
//	float3 gCameraPos;
//
//	/// The distance of the near clip plane
//	float gNearDist;
//	/// The distance of the far clip plane
//	float gFarDist;
//	/// Precomputed value used to convert depth to actual distance.
//	/// gPlaneX = -far / (far - near)
//	/// distance = gPlaneY / (gPlaneX + depth)
//	float gPlaneX;
//	/// Precomputed value used to convert depth to actual distance.
//	/// gPlaneY = -far * near/ (far - near)
//	/// distance = gPlaneY / (gPlaneX + depth)
//	float gPlaneY;
//}
//
//cbuffer cbMisc : register(b5)
//{
//	/// The difference between texel and pixel
//	float2 gHalfPixel;
//	/// An offset to be used when comparing depths for shadow calculations
//	float gShadowDepthOffset;
//	/// An offset to be used to find the texels in the directional light shadow 
//	/// map around the one being sampled.
//	float gShadowDirOffset;
//
//	/// Kernel used for up and down sampling x4
//	float2 TexelCoordsDownFilterX4[16];
//}

#endif //CONSTANT_BUFFERS_HLSLI