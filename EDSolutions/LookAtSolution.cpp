#include "../FSGDGame/LookAt.h"
#include "../EDGameCore/Game.h"
#include "../EDGameCore/GameObject.h"

using namespace EDGameCore;

void LookAt::LookAtSolution()
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

		// Get the world-space vector from "looker" to "target"
		mat.ZAxis = target_pos - looker_pos;

		// If "looker" has a parent transform, object-space != world-space.
		// New Z is in world-space, so transform by inverse of parent world-matrix
		// to find it's equivalent vector in object-space
		Transform* parentTransform = looker_transform->GetParent();
		if( parentTransform != 0 )
			InvTransformVector( mat.ZAxis, mat.ZAxis, parentTransform->GetWorldMatrix() );
		
		// Recalculate the object-space matrix for the "looker"
		CrossProduct(mat.XAxis, Float3(0.0f, 1.0f, 0.0f), mat.ZAxis);
		CrossProduct(mat.YAxis, mat.ZAxis, mat.XAxis);
		
		mat.XAxis.normalize();
		mat.YAxis.normalize();
		mat.ZAxis.normalize();

		looker_transform->SetLocalMatrix(mat);
	}
}

/*
#include <assert.h>
#include "precompiled.h"

#include "../FSGDGame/LookAtBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDUtilities/PrintConsole.h"

using namespace EDMath;
using namespace EDGOCA;
using namespace EDUtilities;

namespace Behaviors
{
	void LookAtBehavior::OnUpdateSolution(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"LookAtBehavior::OnUpdateSolution");
			added = true;
		}


		assert(reinterpret_cast<LookAtBehavior *>(pInvokingBehavior));

		LookAtBehavior *lookAtPtr = (LookAtBehavior *)pInvokingBehavior;
		
		if( lookAtPtr->targetObjPtr == 0 )
			return;

		// get the position of the target, the looker, and the world's up vector
		Float3 targetPos = lookAtPtr->targetObjPtr->GetWorldTransform().WAxis;
		Float4x4 result = lookAtPtr->gameObject->GetLocalTransform();
		Float3 worldUp(0, 1, 0);

		// z axis assigned to the vector that points towards the target
		result.ZAxis = (targetPos - result.WAxis).normalize();

		// if we cross world up with z vector we get the perpendicular x axis
		CrossProduct(result.XAxis, worldUp, result.ZAxis);
		result.XAxis.normalize();

		// if we cross the correct z and x axis the perpendicular should be the y axis
		CrossProduct(result.YAxis, result.ZAxis, result.XAxis);
		result.YAxis.normalize();

		// assign the looking ojbect to the calculated matrix
		lookAtPtr->gameObject->SetLocalTransform(result);
	}
}
*/