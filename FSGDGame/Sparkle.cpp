#include "Sparkle.h"

#include "../EDGameCore/Transform.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Light.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDGameCore/Game.h"

inline float RandomFloat(float min, float max)
{
	return (((rand()/(float)RAND_MAX)*((max)-(min)))+(min));
}

void randomPos(Float3& p)
{
	for(uint32_t i = 0; i < 3; ++i)
		p.v[i] = RandomFloat( -1.0f, 1.0f );

	if( p.squaredMagnitude() < 0.000001f )
		randomPos(p);
	else
		p.normalize() *= RandomFloat( 0.1f, 0.15f );
}

Sparkle::Sparkle(void)
{
	for(uint32_t i = 0; i < SPARKLE_COUNT; ++i)
	{
		randomPos( points[i][0] );
		points[i][1] = points[i][0] * 2.0f;
	}
}

void Sparkle::Update(void)
{
	XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);

	EDGameCore::ILight* light = GetLight();
	if( !light || light->GetLightType() != EDGameCore::ILight::POINT )
		return;

	EDGameCore::IPointLight* pointLight = (EDGameCore::IPointLight*)light;

	color.x = light->GetColor().x;
	color.y = light->GetColor().y;
	color.z = light->GetColor().z;

	float rSq = pointLight->GetRange() * 0.25f;
	rSq *= rSq;

	const Float4x4& worldMat = GetTransform()->GetWorldMatrix();

	for(uint32_t i = 0; i < SPARKLE_COUNT; ++i)
	{
		Float3 delta = (points[i][1] - points[i][0]) * EDGameCore::Game::GetDeltaTime() * 25.0f;

		points[i][1] += delta;
		points[i][0] += delta;

		float sqMag = points[i][0].squaredMagnitude();

		if( sqMag > rSq )
		{
			randomPos(points[i][0]);
			points[i][1] = points[i][0] * 2.0f;
			sqMag = points[i][0].squaredMagnitude();
		}

		float ratio = 1.0f - (sqMag / rSq);

		Float3 newPoints[2];
		
		TransformPoint( newPoints[0], points[i][0], worldMat );
		TransformPoint( newPoints[1], points[i][1], worldMat );


		EDRendererD3D::DebugRenderer::GetInstance()->DrawSegment( newPoints[0], worldMat.WAxis, XMFLOAT4(color.x + 0.75f, color.y + 0.75f, color.z + 0.75f, ratio*ratio) );
		EDRendererD3D::DebugRenderer::GetInstance()->DrawSegment( newPoints[0], newPoints[1], XMFLOAT4(color.x + 0.75f, color.y + 0.75f, color.z + 0.75f, ratio) );
	}
}