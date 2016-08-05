#include "../FSGDGame/MouseLookController.h"
#include "../EDGameCore/Transform.h"

void MouseLookController::MouseLookControllerSolution(POINT& _new_pos)
{
	float x_delta = (_new_pos.x - cursorPos.x) * MOUSE_LOOK_SPEED_MODIFIER;
	float y_delta = (_new_pos.y - cursorPos.y) * MOUSE_LOOK_SPEED_MODIFIER;

	Float4x4 camera_mat = GetGameObject()->GetTransform()->GetLocalMatrix();
	
	camera_mat.rotateLocalY(x_delta);
	camera_mat.rotateLocalX(y_delta);

	CrossProduct(camera_mat.XAxis, Float3(0.0f, 1.0f, 0.0f), camera_mat.ZAxis);
	CrossProduct(camera_mat.YAxis, camera_mat.ZAxis, camera_mat.XAxis);

	camera_mat.XAxis.normalize();
	camera_mat.YAxis.normalize();
	camera_mat.ZAxis.normalize();

	GetGameObject()->GetTransform()->SetLocalMatrix(camera_mat);
}
