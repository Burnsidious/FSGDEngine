#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Light.h"

namespace EDGameCore
{
	IRenderer::IRenderer(void) : renderableTreeLeaf(0), visibleSetNext(0), containingTree(0)
	{
		memset( lightInfluences, 0, sizeof(lightInfluences) );
	}

	IRenderer::~IRenderer()
	{
		if( containingTree != 0 )
			containingTree->Remove( renderableTreeLeaf );

		renderableTreeLeaf = 0;
		containingTree = 0;

		GameObject* gameObject = GetGameObject();
		if( gameObject && gameObject->GetRenderer() == this )
			gameObject->SetRenderer((IRenderer*)nullptr);
	}

	void IRenderer::AddLight(ILight* light)
	{
		float localIntensity = light->GetIntensity();
		
		if( light->GetLightType() == ILight::POINT )
		{
			GameObject* gameObject = GetGameObject();
			Float3 pos = gameObject->GetTransform()->GetWorldMatrix().WAxis;
			Float3 lightPos = light->GetGameObject()->GetTransform()->GetWorldMatrix().WAxis;

			float invDistSq = 1.0f / (pos - lightPos).squaredMagnitude();

			if( invDistSq < 1.0f )
				localIntensity *= invDistSq;
		}

		const float DIRECTIONAL_LIGHT_BIAS = 1.5f;
		float bias1 = (light->GetLightType() != ILight::POINT) ? DIRECTIONAL_LIGHT_BIAS : 1.0f;

		int i = 0;
		for(; i < MAX_LIGHTS; ++i)
		{
			if( lightInfluences[i].light == 0 )
				break;

			float bias2 = (lightInfluences[i].light->GetLightType() != ILight::POINT) ? DIRECTIONAL_LIGHT_BIAS : 1.0f;

			float i1 = (localIntensity * bias1);
			float i2 = (lightInfluences[i].localIntensity * bias2);

			float diff = i1 - i2;

			if( diff > 0.01f )
				break;

			if( diff > -0.01f )
			{
				if( light < lightInfluences[i].light )
					break;
			}
		}

		if( i != MAX_LIGHTS )
		{
			for(int j = MAX_LIGHTS - 1; j > i; --j)
				lightInfluences[j] = lightInfluences[j-1];

			lightInfluences[i].light = light;
			lightInfluences[i].localIntensity = localIntensity;
		}
	}

	void IRenderer::ClearLights(void)
	{
		memset( lightInfluences, 0, sizeof(lightInfluences) );
	}
}