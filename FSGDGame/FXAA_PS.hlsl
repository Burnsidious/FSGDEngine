#ifndef FXAA_FX
#define FXAA_FX

#include "VertexLayouts.hlsli"
#include "TexturesAndSamplers.hlsli"
#include "../EDRendererD3D/ConstantBuffers.h"

#define FXAA_QUALITY 3

#if(3 == FXAA_QUALITY )
//FXAA_EDGE_THRESHOLD - The minimum amount of local contrast required 
#define FXAA_EDGE_THRESHOLD      (1.0/8.0)
//FXAA_EDGE_THRESHOLD_MIN - Trims the algorithm from processing darks.
// No point processing places too dark to notice the change
#define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
//FXAA_SUBPIX_TRIM - Controls sub-pixel aliasing removal.
#define FXAA_SUBPIX_TRIM         (1.0/4.0)
//FXAA_SUBPIX_CAP - Insures fine detail is not completely removed.
#define FXAA_SUBPIX_CAP          (3.0/4.0)
// FXAA_SEARCH_THRESHOLD - Controls when to stop searching.
#define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
//FXAA_SEARCH_STEPS - Maximum number of search steps for end of span.
#define FXAA_SEARCH_STEPS        16
#endif
#if(2 == FXAA_QUALITY )
#define FXAA_EDGE_THRESHOLD      (1.0/8.0)
#define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
#define FXAA_SEARCH_STEPS        8
#define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
#define FXAA_SUBPIX_CAP          (3.0/4.0)
#define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif
#if(1 == FXAA_QUALITY )
#define FXAA_EDGE_THRESHOLD      (1.0/8.0)
#define FXAA_EDGE_THRESHOLD_MIN  (1.0/16.0)
#define FXAA_SEARCH_STEPS        4
#define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
#define FXAA_SUBPIX_CAP          (3.0/4.0)
#define FXAA_SUBPIX_TRIM         (1.0/4.0)
#endif

#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))

//float4 tex2DOffset(sampler2D texSampler, float2 pos, float2 offset)
float4 tex2DOffset(Texture2D tex, float2 pos, float2 offset)
{
	//return tex2D(texSampler, pos + offset * (gHalfPixel * 2));
	return tex.Sample(anisoClampSampler, pos + offset * (gHalfPixel * 2));
}

// fast approximate luminance calculation stolen from nVidia FXAA
float calcLuminance(float3 rgb)
{
	return rgb.y * (0.587/0.299) + rgb.x;
}

float3 FxaaLerp3(float3 a, float3 b, float amountOfA) 
{
    return (float3(-amountOfA, -amountOfA, -amountOfA) * b) + 
        ((a * float3(amountOfA, amountOfA, amountOfA)) + b); 
} 

float4 main(TwoDVertexOut inVert) : SV_TARGET0
{
	float2 pixelSize = gHalfPixel * 2;
	float2 texCoord = inVert.texCoord;

	// Get colors in cross pattern around fragment
	float3 colorCenter = postSourceMap.Sample(linearClampSampler, texCoord).rgb;
	float3 colorN = tex2DOffset(postSourceMap, texCoord, float2(0, -1)).rgb;
	float3 colorS = tex2DOffset(postSourceMap, texCoord, float2(0, 1)).rgb;
	float3 colorE = tex2DOffset(postSourceMap, texCoord, float2(1, 0)).rgb;
	float3 colorW = tex2DOffset(postSourceMap, texCoord, float2(-1, 0)).rgb;

	// Get lumanance for each color
	float lumaN = calcLuminance(colorN);
    float lumaW = calcLuminance(colorW);
    float lumaCenter = calcLuminance(colorCenter);
    float lumaE = calcLuminance(colorE);
    float lumaS = calcLuminance(colorS);

	// Debug only
	float lumaO = lumaCenter / (1.0 + (0.587/0.299));

	// Calculatate change in luminance
	float rangeMin = min(lumaCenter, min(min(lumaN, lumaW), min(lumaS, lumaE)));
    float rangeMax = max(lumaCenter, max(max(lumaN, lumaW), max(lumaS, lumaE)));
    float range = rangeMax - rangeMin;

	// Test if this is an edge
    if(range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD)) 
	{
		// Early return for non-edges
		return float4(colorCenter, 1);
	}

	// Draw edges
	//return float4(0, 0, 0, 0);

	// Compute low-pass
	float lumiLowPass = (lumaN + lumaW + lumaE + lumaS) * 0.25;
	float lumiLowPassRange = abs(lumiLowPass - lumaCenter);
	float blendL = max(0.0, (lumiLowPassRange / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE; 
    blendL = min(FXAA_SUBPIX_CAP, blendL);

	// Draw aliasing representation
	//FXAA_DEBUG_PASSTHROUGH
	//return float4(1.0, blendL/FXAA_SUBPIX_CAP, 0.0, 0);

	///////////////////////////////////////////////////////////////
	// Determine if the edge is vertical or horizontal
	float3 colorNW = tex2DOffset(postSourceMap, texCoord, float2(-1, -1)).rgb;
	float3 colorNE = tex2DOffset(postSourceMap, texCoord, float2(1, -1)).rgb;
	float3 colorSW = tex2DOffset(postSourceMap, texCoord, float2(-1, 1)).rgb;
	float3 colorSE = tex2DOffset(postSourceMap, texCoord, float2(1, 1)).rgb;

	float3 colorAverage = (colorN + colorW + colorCenter + colorE + colorS + 
		colorNW + colorNE + colorSW + colorSE) / 9.0;

    float lumaNW = calcLuminance(colorNW);
    float lumaNE = calcLuminance(colorNE);
    float lumaSW = calcLuminance(colorSW);
    float lumaSE = calcLuminance(colorSE);

	float edgeVert = 
        abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
        abs((0.50 * lumaW ) + (-1.0 * lumaCenter) + (0.50 * lumaE )) +
        abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
    float edgeHorz = 
        abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
        abs((0.50 * lumaN ) + (-1.0 * lumaCenter) + (0.50 * lumaS )) +
        abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
    bool horzSpan = edgeHorz >= edgeVert;

	// Draw horizontal edges in yellow and vertical in blue
	//FXAA_DEBUG_HORZVERT
	//if(horzSpan) 
	//	return float4(1.0, 0.75, 0.0, 0);
 //   else
	//	return float4(0.0, 0.50, 1.0, 0);

	///////////////////////////////////////////////////////////////
	// Choose side of pixel where the gradient is highest
	// Variables used in abstracted method for vertical or horizontal
	// North and South used for east and west in vertical case
	float lengthSign = horzSpan ? -pixelSize.y : -pixelSize.x;
    if(!horzSpan) 
		lumaN = lumaW;
    if(!horzSpan) 
		lumaS = lumaE;
    float gradientN = abs(lumaN - lumaCenter);
    float gradientS = abs(lumaS - lumaCenter);
    lumaN = (lumaN + lumaCenter) * 0.5;
    lumaS = (lumaS + lumaCenter) * 0.5;

	bool pairN = gradientN >= gradientS;

	// If contrast highest north or west draw in blue, else in green
	//FXAA_DEBUG_PAIR
	//if(pairN) 
	//	return float4(0.0, 0.0, 1.0, 0);
 //   else      
	//	return float4(0.0, 1.0, 0.0, 0);
    
	// Resuse north variables for either case N or S
	if(!pairN) 
		lumaN = lumaS;
    if(!pairN) 
		gradientN = gradientS;
    if(!pairN) 
		lengthSign *= -1.0;
    float2 posN;
    posN.x = texCoord.x + (horzSpan ? 0.0 : lengthSign * 0.5);
    posN.y = texCoord.y + (horzSpan ? lengthSign * 0.5 : 0.0);
	
	///////////////////////////////////////////////////////////////
	// Search in each direction until the change in luminance is 
	// out of acceptable range
    gradientN *= FXAA_SEARCH_THRESHOLD;

    float2 posP = posN;
    float2 offNP = horzSpan ? 
        float2(pixelSize.x, 0.0) :
        float2(0.0f, pixelSize.y);
    float lumaEndN = lumaN;
    float lumaEndP = lumaN;
    bool doneN = false;
    bool doneP = false;

    posN += offNP * float2(-1.0, -1.0);
    posP += offNP * float2( 1.0,  1.0);
    
	for(int i = 0; i < FXAA_SEARCH_STEPS; ++i)
	{
		// Get luminance for next fragment in each direction
		if(!doneN) 
			lumaEndN = calcLuminance(postSourceMap.SampleLevel(linearClampSampler, posN.xy, 0).xyz);
		if(!doneP) 
			lumaEndP = calcLuminance(postSourceMap.SampleLevel(linearClampSampler, posP.xy, 0 ).xyz);

		// Are we done, have we found a change greater than our threshold?
        doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);
        doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);
        // Are we done in both directions?
		if(doneN && doneP) 
			//i = FXAA_SEARCH_STEPS;
			break;
		// Next fragment
        if(!doneN) posN -= offNP;
        if(!doneP) posP += offNP;
	}

	///////////////////////////////////////////////////////////////
	// Check if center pixel is on the north or south end 
	// of the edge
    float dstN = horzSpan ? texCoord.x - posN.x : texCoord.y - posN.y;
    float dstP = horzSpan ? posP.x - texCoord.x : posP.y - texCoord.y;
    bool directionN = dstN < dstP;
	// Draw negative edges in red, positive in blue        
	//if(directionN) 
	//	return float4(1.0, 0.0, 0.0, 0);
 //   else           
	//	return float4(0.0, 0.0, 1.0, 0);
    lumaEndN = directionN ? lumaEndN : lumaEndP;

	// Check if this pixel should not be filtered
	if(((lumaCenter - lumaN) < 0.0) == ((lumaEndN - lumaN) < 0.0)) 
        lengthSign = 0.0;

	// Filter the pixel
	    float spanLength = (dstP + dstN);
    dstN = directionN ? dstN : dstP;
    float subPixelOffset = (0.5 + (dstN * (-1.0/spanLength))) * lengthSign;

	// Draw supixel shifts
  //  float ox = horzSpan ? 0.0 : subPixelOffset*2.0/pixelSize.x;
  //  float oy = horzSpan ? subPixelOffset*2.0/pixelSize.y : 0.0;
  //  if(ox < 0.0) 
		//return float4(FxaaLerp3(float3(lumaO, lumaO, lumaO), float3(1.0, 0.0, 0.0), -ox), 0);
  //  if(ox > 0.0) 
		//return float4(FxaaLerp3(float3(lumaO, lumaO, lumaO), float3(0.0, 0.0, 1.0),  ox), 0);
  //  if(oy < 0.0) 
		//return float4(FxaaLerp3(float3(lumaO, lumaO, lumaO), float3(1.0, 0.6, 0.2), -oy), 0);
  //  if(oy > 0.0) 
		//return float4(FxaaLerp3(float3(lumaO, lumaO, lumaO), float3(0.2, 0.6, 1.0),  oy), 0);
  //  return float4(lumaO, lumaO, lumaO, lumaO);
    float3 rgbF = postSourceMap.Sample(linearClampSampler, float2(
		texCoord.x + (horzSpan ? 0.0 : subPixelOffset),
		texCoord.y + (horzSpan ? subPixelOffset : 0.0))).xyz;
       
    return float4(FxaaLerp3(colorAverage, rgbF, blendL), 0); 
	//return lumaCenter / (1.0 + (0.587/0.299));
}

#endif