#include "../FSGDGame/TurnTo.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/Game.h"

using namespace EDGameCore;

void TurnTo::TurnToSolution()
{
	Transform* looker_transform = GetGameObject()->GetTransform();
	Transform* target_transform = nullptr;

	if (target != nullptr)
	{
		target_transform = target->GetTransform();
		Float4x4 mat = looker_transform->GetLocalMatrix();

		// Get WORLD-SPACE positions
		Float3 looker_pos = looker_transform->GetWorldMatrix().WAxis;
		Float3 target_pos = target_transform->GetWorldMatrix().WAxis;		
		Float3 to_target =  target_pos - looker_pos;

		float up_down = DotProduct(mat.YAxis, to_target);
		float left_right = DotProduct(mat.XAxis, to_target);

		if (up_down > DEAD_ZONE || up_down < -DEAD_ZONE)
			mat.rotateLocalX(up_down * turn_speed * Game::GetDeltaTime());
		if (left_right > DEAD_ZONE || left_right < -DEAD_ZONE)
			mat.rotateLocalY(left_right * -turn_speed * Game::GetDeltaTime());		
		
		// Recalculate the object-space matrix for the "looker"
		CrossProduct(mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis);
		CrossProduct(mat.YAxis, mat.ZAxis, mat.XAxis);
		
		mat.XAxis.normalize();
		mat.YAxis.normalize();
		mat.ZAxis.normalize();

		looker_transform->SetLocalMatrix(mat);
	}
}
