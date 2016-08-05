#include "precompiled.h"



#include "../FSGDGame/RenderController.h"
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
#include "../EDMath/Float3.h"
using namespace EDMath;

void RenderController::BuildDirShadowProjectionSolution(const EDMath::Aabb &aabb)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Shadow Mapping", Float3(1.0f, 0.0f, 0.0f), 
			L"Build Directional Shadow Projection Matrix");
		added = true;
	}

	Float3 vmax = aabb.Max();
	Float3 vmin = aabb.Min();

	static float nearVal = 0.01f;
	lightProjection = *(Float4x4 *)&XMMatrixOrthographicLH(vmax.x-vmin.x,
		vmax.y - vmin.y, nearVal, (vmax.z - vmin.z) + 0.01f);
}

void RenderController::BuildDirShadowViewProjectionSolution(const EDMath::Aabb &aabb, 
	EDRendererD3D::DirectionalLightWithShadow *lightPtr, unsigned int index)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Shadow Mapping", Float3(1.0f, 0.0f, 0.0f), 
			L"Build Directional Shadow ViewProjection Matrix");
		added = true;
	}

	XMFLOAT4X4 lightWorld = *lightPtr->GetWorldMatrixPtr();

	Float3 dir(lightWorld._31, lightWorld._32, lightWorld._33);
	Float3 pos;

	pos = aabb.center - (dir * aabb.extents.x);

	lightWorld._41 = pos.x;
	lightWorld._42 = pos.y;
	lightWorld._43 = pos.z;

	XMVECTOR det;
	XMMATRIX tempXMMat = XMLoadFloat4x4(&lightWorld);
	lightView = *(Float4x4 *)&XMMatrixInverse(&det, tempXMMat);

	////////////////////////////////////////////////////
	// Set up light view projection
	Multiply(*(Float4x4 *)&dirViewProjection[index], lightView, lightProjection);
}

void RenderController::BuildDirShadowShimmerFixSolution(unsigned int index)
{
	static bool added = false;
	if(!added)
	{
		PrintConsole::GetReference().AddSolution(L"ED2", "Shadow Mapping", Float3(1.0f, 0.0f, 0.0f), 
			L"Fix Directional Shadow shimmering");
		added = true;
	}
	
	////////////////////////////////////////////////////
	// Cancel out sub-texel translation

	// Find a point in light space
	XMVECTOR lightSpacePoint = {0, 0, 0};
	//D3DXVec3TransformCoord(&lightSpacePoint, &lightSpacePoint, &dirViewProjection);
	
	lightSpacePoint = XMVector3TransformCoord(lightSpacePoint, XMLoadFloat4x4(&dirViewProjection[index]));

	// Find nearest shadow map texel.
	float texCoordX = lightSpacePoint.m128_f32[0] * theDirectionalShadowSize * 0.5f;
	float texCoordY = lightSpacePoint.m128_f32[1] * theDirectionalShadowSize * 0.5f;

	///Lab doc way
	// Find the nearest actual texel by rounding
	//float texCoordRoundedX = floor(texCoordX + 0.5f);
	//float texCoordRoundedY = floor(texCoordY + 0.5f);

	// The difference between texCoordRounded and texCoord will be the amount 
	// for translation we need cancel out to prevent shimmering, ensuring we 
	// always move in whole texels
	//float dx = texCoordRoundedX - texCoordX;
	//float dy = texCoordRoundedY - texCoordY;

	// easier way explained in lecture
	float dx = 1.f - (texCoordX - int(texCoordX));
	float dy = 1.f - (texCoordY - int(texCoordY));

	// Transform dx, dy back to light space
	dx /= theDirectionalShadowSize * 0.5f;
	dy /= theDirectionalShadowSize * 0.5f;

	XMMATRIX mRound = XMMatrixTranslation(dx, dy, 0);
	Multiply(*(Float4x4 *)&dirViewProjection[index], *(Float4x4 *)&dirViewProjection[index],
		*(Float4x4 *)&mRound);
}
