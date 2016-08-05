#ifndef GBUFFERS_SHARED_HLSLI
#define GBUFFERS_SHARED_HLSLI

GBufferFragOut CreateGBufferOutput(float3 normalW, float2 texCoord, float2 depthDiv)
{
	GBufferFragOut output = (GBufferFragOut)0;
	// SOLUTION_BEGIN
	output.normal.xyz = normalize(normalW);
	output.normal = output.normal * .5f + .5f;
	output.normal.w = 0;

	output.diffuse = diffuseMap.Sample(anisoWrapSampler, texCoord);

	if (output.diffuse.a < MIN_ALPHA_KEEP)
		discard;

	output.diffuse.a = ambientMap.Sample(linearWrapSampler, texCoord).r;

	output.specular = specularMap.Sample(linearWrapSampler, texCoord);

	output.depth = depthDiv.x / depthDiv.y;

	// SOLUTION_END
	return output;
}

#endif //GBUFFERS_SHARED_HLSLI