#ifndef TEXTURES_AND_SAMPLERS_HLSLI
#define TEXTURES_AND_SAMPLERS_HLSLI

#define MIN_ALPHA_KEEP .8

///////////////////////////////////////////////////////////
// Common object or context based textures
Texture2D ambientMap			: register(t0);
Texture2D diffuseMap			: register(t1);
Texture2D specularMap			: register(t2);
Texture2D emmisiveMap			: register(t3);
Texture2D normalMap				: register(t4);
TextureCube reflectedMap		: register(t5);

// Post-process textures
Texture2D prePostMap			: register(t7);
Texture2D postSourceMap			: register(t8);

// GBuffers
Texture2D diffuseGBuffer		: register(t9);
Texture2D specularGBuffer		: register(t10);
Texture2D normalGBuffer			: register(t11);
Texture2D depthGBuffer			: register(t12);

// shadows
Texture2D directional0ShadowMap[3] : register(t13);

Texture2D spot0ShadowMap		: register(t14);
TextureCube point0ShadowMap		: register(t15);

// Terrain Textures
Texture2D courseHeightMap		: register(t0);
Texture2D diffuseMap1			: register(t1);
Texture2D diffuseMap2			: register(t2);
Texture2D diffuseNoise			: register(t4);

// Note: We are in dange of running out of texture registers.  Currently, t0-t5 do not need any other 
// registers to be active at the same time as them, and could be reused. Also, shadow and 
// post-processing registers do not need to be active at the same time.


///////////////////////////////////////////////////////////
// Common samplers
SamplerState spriteBatchSampler						: register(s0);
SamplerState anisoWrapSampler						: register(s1);
SamplerState anisoClampSampler						: register(s2);
SamplerState pointWrapSampler						: register(s3);
SamplerState pointClampSampler						: register(s4);
SamplerState linearWrapSampler						: register(s5);
SamplerState linearClampSampler						: register(s6);
SamplerComparisonState linearBorderCompareSampler	: register(s7);

#endif //TEXTURES_AND_SAMPLERS_HLSLI