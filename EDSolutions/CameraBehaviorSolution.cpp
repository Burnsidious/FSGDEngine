#include "precompiled.h"

#include "../EDGameCore/Camera.h"

namespace EDGameCore
{
	const Float4x4& Camera::GetViewMatrixSolution(void)
	{
		const Float4x4& world = GetGameObject()->GetTransform()->GetWorldMatrix();

		viewMatrix = world;
 
		// all of this could be done with D3DXMatrixLookAtLH, but lets do it by hand!
		// grab each axis
		Float3 f3X = viewMatrix.XAxis;
		Float3 f3Y = viewMatrix.YAxis;
		Float3 f3Z = viewMatrix.ZAxis;
		Float3 f3W = viewMatrix.WAxis;

		// new W axis is the negative dot of the old W axis with X,Y,Z
		viewMatrix.Wx = -f3X.dotProduct(f3W);
		viewMatrix.Wy = -f3Y.dotProduct(f3W);
		viewMatrix.Wz = -f3Z.dotProduct(f3W);
 
		// transpose x,y,z axis
		viewMatrix.transpose3x3();

		return viewMatrix;
	}
}



/*
#include "precompiled.h"

#include "../FSGDGame/CameraBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDUtilities/PrintConsole.h"

using namespace EDMath;
using namespace EDGOCA;
using namespace EDUtilities;

namespace Behaviors
{
	void CameraBehavior::OnUpdateSolution(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"CameraBehavior::OnUpdateSolution");
			added = true;
		}

		CameraBehavior *cameraBehavior = (CameraBehavior*)pInvokingBehavior;
		Float4x4 world = cameraBehavior->gameObject->GetWorldTransform();
		Float4x4 view = world;
 
		// all of this could be done with D3DXMatrixLookAtLH, but lets do it by hand!
		// grab each axis
		Float3 f3X = view.XAxis;
		Float3 f3Y = view.YAxis;
		Float3 f3Z = view.ZAxis;
		Float3 f3W = view.WAxis;
 
		// new W axis is the negative dot of the old W axis with X,Y,Z
		view.Wx = -f3X.dotProduct(f3W);
		view.Wy = -f3Y.dotProduct(f3W);
		view.Wz = -f3Z.dotProduct(f3W);
 
		// transpose x,y,z axis
		view.transpose3x3();

		// update viewport manager with world matrix and calculated view matrix
		EDRendererD3D::ViewPortManager::GetReference().UpdateViewPort(
			cameraBehavior->viewPortIndex, world, view, cameraBehavior->projection, 
			cameraBehavior->nearClip, cameraBehavior->farClip);
	}
}
*/
