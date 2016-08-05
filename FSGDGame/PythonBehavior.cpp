#include <list>
#include "PythonBehavior.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"

using namespace std;
using namespace EDGameCore;

PythonBehavior::PythonBehavior()
{
	target = nullptr;
	targetId = 0;
}

PythonBehavior::PythonBehavior(PythonBehavior& _rhs)
{
	target = nullptr;
	targetId = 0;
	module.LoadModule(_rhs.GetModuleName());
}

PythonBehavior::~PythonBehavior()
{
	module.UnloadModule();
}

void PythonBehavior::LateUpdate()
{
	Transform* looker_transform = GetGameObject()->GetTransform();
	Transform* target_transform = nullptr;
	Float4x4 mat = looker_transform->GetLocalMatrix();
	float xspeed = 0.0f;
	float zspeed = 0.0f;

	// Finds the target based on its instance id each update 
	// to avoid having a dangling pointer to an object that might have been destroyed.
	target = GameObject::GetGameObjectInstance(targetId);
	
	if (module.IsValid())
	{
		module.CallFunction(string("borg_behavior"));
		module.GetValue(string("xspeed"), xspeed);
		module.GetValue(string("zspeed"), zspeed);
	}

	mat.translateLocal(Float3(xspeed, 0.0, zspeed));
	looker_transform->SetLocalMatrix(mat);
}

void PythonBehavior::LoadState(TiXmlElement* xmlElement)
{
	// Get the name of the target to look-at
	TiXmlElement* targetEle = xmlElement->FirstChildElement("TargetName");
	if( targetEle != 0 &&
		targetEle->GetText() != 0 &&
		strlen( targetEle->GetText() ) != 0 )
		targetName = targetEle->GetText();

	module.LoadModule("borg_entity");
}

void PythonBehavior::Awake(void)
{
	// Find the first GameObject named targetName.
	GameObject* obj = GameObject::Find( targetName.c_str() );

	if( obj != 0 )
		targetId = obj->GetInstanceId();
}
