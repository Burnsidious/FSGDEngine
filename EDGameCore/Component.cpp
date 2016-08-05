#include "Component.h"
#include "GameObject.h"
namespace EDGameCore
{
	const char* Component::GetName(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetName();
	}

	Animation* Component::GetAnimation(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetAnimation();
	}

	Transform* Component::GetTransform(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetTransform();
	}

	IRenderer* Component::GetRenderer(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetRenderer();
	}

	RigidBody* Component::GetRigidBody(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetRigidBody();
	}

	ILight* Component::GetLight(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetLight();
	}

	ICollider* Component::GetCollider(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetCollider();
	}

	Camera* Component::GetCamera(void)
	{
		return (gameObject == 0) ? 0 : gameObject->GetCamera();
	}

	const Animation* Component::GetAnimation(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetAnimation();
	}

	const Transform* Component::GetTransform(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetTransform();
	}

	const IRenderer* Component::GetRenderer(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetRenderer();
	}

	const RigidBody* Component::GetRigidBody(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetRigidBody();
	}

	const ILight* Component::GetLight(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetLight();
	}

	const ICollider* Component::GetCollider(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetCollider();
	}

	const Camera* Component::GetCamera(void)const 
	{
		return (gameObject == 0) ? 0 : gameObject->GetCamera();
	}

	bool Component::isEnabledInHierarchy(void)const
	{
		if( !isEnabled() )
			return false;

		const GameObject* gameObj = GetGameObject();
		
		if( gameObj == 0 )
			return true;

		return gameObj->isEnabledInHierarchy();
	}

	Component::~Component()
	{
	}
}