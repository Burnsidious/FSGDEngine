#include "Light.h"
#include "Game.h"
#include "Renderer.h"
#include "LayerManager.h"

namespace EDGameCore
{
	void IPointLight::Update(void)
	{
		GetAabb();
	}

	bool IPointLight::CullRenderable(IRenderer* renderable)
	{
		if( !renderable->isEnabledInHierarchy() )
			return false;

		if( (Game::GetLayerMask( renderable->GetGameObject()->GetLayer() ) & cullMask) == 0 )
			return false;

		if( TestAabb( renderable->GetAabb() ) )
			return true;

		return false;
	}

	IPointLight::~IPointLight()
	{
		if( containingTree != 0 )
			containingTree->Remove( lightTreeLeaf );

		lightTreeLeaf = 0;
		containingTree = 0;

		GameObject* gameObject = GetGameObject();
		if( gameObject && GetGameObject()->GetLight() == this )
			gameObject->SetLight((ILight*)nullptr);
	}

	const EDMath::Aabb& IPointLight::GetAabb(void)
	{
		worldAabb.center = GetGameObject()->GetTransform()->GetWorldMatrix().WAxis;
		worldAabb.extents.x = worldAabb.extents.y = worldAabb.extents.z = range;

		return worldAabb;
	}

	bool IPointLight::TestAabb(const EDMath::Aabb& aabb)
	{
		EDMath::Sphere sphere;
		sphere.center = GetAabb().center;
		sphere.radius = range;

		return EDCollision::SphereToAabb( sphere, aabb );
	}

	bool ISpotLight::TestAabb(const EDMath::Aabb& aabb)
	{
		return EDCollision::AabbToAabb( worldAabb, aabb );		
	}

	void ISpotLight::SetSpotAngle(float s)
	{
		spotAngle = s;

		radius = tanf(0.5f*spotAngle) * range;
	}

	void ISpotLight::SetRange(float r)
	{
		IPointLight::SetRange(r);

		radius = tanf(0.5f*spotAngle) * range;
	}

	const EDMath::Aabb& ISpotLight::GetAabb(void)
	{
		Aabb localAabb;
		localAabb.center = Float3( 0.0f, 0.0f, range*0.5f );
		localAabb.extents = Float3( radius, radius, range*0.5f );

		TransformAabb( worldAabb, localAabb, GetGameObject()->GetTransform()->GetWorldMatrix() );

		return worldAabb;
	}

	void IDirectionalLight::Update(void)
	{
		direction = GetGameObject()->GetTransform()->GetWorldMatrix().ZAxis;
	}

	bool IDirectionalLight::CullRenderable(IRenderer* renderable)
	{
		if( !renderable->isEnabledInHierarchy() )
			return false;

		if( (Game::GetLayerMask( renderable->GetGameObject()->GetLayer() ) & cullMask) == 0 )
			return false;
		
		return true;
	}

	bool IDirectionalLight::TestAabb(const EDMath::Aabb& aabb)
	{
		return EDCollision::AabbToAabb( GetAabb(), aabb );
	}

	IDirectionalLight::~IDirectionalLight()
	{
		if( containingList != 0 )
			containingList->remove( this );

		GameObject* gameObject = GetGameObject();
		if( gameObject && GetGameObject()->GetLight() == this )
			gameObject->SetLight((ILight*)nullptr);
	}
}