#include "../FSGDGame/PIDFollower.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"

using namespace EDGameCore;

void PIDFollower::PIDFollowerSolution()
{
	Transform* looker_transform = GetGameObject()->GetTransform();
	Transform* target_transform = nullptr;
	float delta_time = EDGameCore::Game::GetDeltaTime();
	float pid_result = 0.0f;

	// Finds the target based on its instance id each update 
	// to avoid having a dangling pointer to an object that might have been destroyed.
	target = GameObject::GetGameObjectInstance(targetId);

	if (target != nullptr)
	{
		Float4x4 looker_mat = looker_transform->GetTransform()->GetLocalMatrix();
		Float3 looker_pos = looker_transform->GetTransform()->GetLocalMatrix().WAxis;
		Float3 target_pos = target->GetTransform()->GetLocalMatrix().WAxis;

		// use PID results to move stalker towards target
		pid_result = y_pid.Update(delta_time, looker_pos.y, target_pos.y + bounce);
		looker_mat.WAxis.y += pid_result;

		pid_result = x_pid.Update(delta_time, looker_pos.x, target_pos.x);
		looker_mat.WAxis.x += pid_result;

		pid_result = z_pid.Update(delta_time, looker_pos.z, target_pos.z);
		looker_mat.WAxis.z += pid_result;

		// perform look at
		looker_mat.ZAxis = target_pos - looker_pos;
		looker_mat.ZAxis.normalize();
		CrossProduct(looker_mat.XAxis, Float3(0.0f, 1.0f, 0.0f), looker_mat.ZAxis);
		looker_mat.XAxis.normalize();
		CrossProduct(looker_mat.YAxis, looker_mat.ZAxis, looker_mat.XAxis);
		looker_mat.YAxis.normalize();
		looker_mat.rotateLocalZ(spin);
		
		spin += (1.0f * delta_time);
		bounce += (sinf(spin) * delta_time);

		looker_transform->GetTransform()->SetLocalMatrix(looker_mat);
	}
}
