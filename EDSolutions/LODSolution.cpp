#include "precompiled.h"
#include <math.h>

#include "../EDGameCore/Camera.h"

#include "../FSGDGame/RenderableSetBehaviorT.h"
#include "../FSGDGame/BaseRenderable.h"
#include "../EDUtilities/PrintConsole.h"
using EDUtilities::PrintConsole;
using namespace DirectX;
using namespace std;
using namespace EDMath;
using namespace EDRendererD3D;
using namespace EDGOCA;


namespace EDGameCore
{
	// extents, renderable, nearDistance, depth, nearPlaneArea
	void Camera::SetLODSolution(IRenderer* renderable, float maxExtent, float nearDistance, 
		float depth, float minArea)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED2", "LOD", Float3(1.0f, 0.0f, 0.0f), 
				L"Camera::SetLODSolution");
			added = true;
		}

		unsigned int numLODs = renderable->GetNumLODs();

		if( numLODs != 1 )
		{
			float projectedRadius = (nearDistance * maxExtent) / depth;
			
			float projectedArea = (float)M_PI * projectedRadius * projectedRadius;

			float areaOccupiedRatio = min( 1.0f, projectedArea / minArea );

			renderable->SetCurrentLOD( (unsigned int)((numLODs-1) * areaOccupiedRatio) );
		}
	}
}

// Old solution, no longer called
//namespace Behaviors
//{
//	template<>
//	void RenderableSetBehaviorT<BaseRenderable>::SetLODSolution( RenderShapeList &shapeList, 
//		GameObject &gameObject)
//	{
//		static bool added = false;
//		if(!added)
//		{
//			PrintConsole::GetReference().AddSolution(L"ED2", "LOD", Float3(1.0f, 0.0f, 0.0f), 
//				L"RenderableSetBehaviorT<BaseRenderable>::SetLODSolution");
//			added = true;
//		}
//		//ViewPortManager &viewPortMan = ViewPortManager::GetReference();
//
//		//float nearPlane = viewPortMan.GetActiveViewNearClip();
//		//Float3 direction = viewPortMan.GetActiveWorld().ZAxis;
//		//Float3 viewer = viewPortMan.GetActiveWorld().WAxis;
//
//		//RenderShapeList::iterator shapeListIter = shapeList.begin();
//		//RenderShapeList::iterator shapeListIterEnd = shapeList.end();
//
//		//for(; shapeListIter != shapeListIterEnd; ++shapeListIter)
//		//{
//		//	RenderShape* pShape = shapeListIter->first;
//		//	pShape->SetWorldMatrix( (XMFLOAT4X4*)&gameObject.GetWorldTransform( shapeListIter->second ) );
//
//		//	const EDMath::Sphere& shapeSphere = pShape->GetSphere();
//
//		//	Float3 toCenter = shapeSphere.center - viewer;
//		//	float sphereProjection = DotProduct( direction, toCenter );
//
//		//	pShape->SetZDepth( sphereProjection );
//		//	pShape->SetCurrentLOD(0);
//
//		//	if( pShape->GetNumLODs() != 1)
//		//	{
//		//		float projectedRadius = (nearPlane * shapeSphere.radius) / sphereProjection;
//
//		//		UINT lod = 2;
//
//		//		for(UINT i = 0; i < pShape->GetNumLODs() - 1 && i < globalNumLODRadii; ++i)
//		//		{
//		//			if(projectedRadius > globalLODRadii[i])
//		//			{
//		//				lod = i;
//		//				if(lod > pShape->GetCurrentLOD())
//		//				{
//		//					if(projectedRadius + globalHysteresis > globalLODRadii[i-1])
//		//						--lod;
//		//				}
//		//				break;
//		//			}
//		//		}
//
//		//		pShape->SetCurrentLOD(lod);
//		//		
//		//	}
//		//	//pShape->SetCurrentLOD(0);
//		//	pShape->AddToContextSet();
//
//		//}
//	}
//}