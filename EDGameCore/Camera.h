#pragma once

#include "Behavior.h"
#include "../EDMath/plane.h"
#include "Rendering.h"
#include "Game.h"
#include "camcomp.h"
#include <list>
#include <set>
using namespace std;

namespace EDGameCore
{
	class IRenderer;
	class IPointLight;

	class Camera : public ComponentT<Camera>
	{
		friend class Game;
		friend class Rendering;
	public:
		Camera(void);
		Camera(const Camera& rhs);

		~Camera();

		ComponentType GetComponentType(void)const{ return CAMERA; }

		virtual void LoadState(TiXmlElement* xmlElement);

		void Update(void);

		const Float4x4& GetProjectionMatrix(void);
		const Float4x4& GetViewMatrix(void);
		const Float4x4& GetViewMatrixSolution(void);
		const Float4x4& GetViewProjectionMatrix(void);

		bool CullRenderable(IRenderer* renderable);
		bool CullLight(ILight* light);

		void SetFieldOfView(float fov)
		{
			fieldOfView = fov;
		}

		float GetFieldOfView(void){ return fieldOfView; }

		void SetNearDistance(float n);

		float GetNearDistance(void){ return nearDistance; }

		void SetFarDistance(float f);

		float GetFarDistance(void){ return farDistance; }

		float GetNearHeight(void){ return nearHeight; }
		float GetNearWidth(void){ return nearWidth; }

		void SetAspectRatio(float asp)
		{
			aspect = asp;
		}

		float GetAspectRatio(void){ return aspect; }

		struct CameraRect
		{
			float minX;
			float minY;
			float maxX;
			float maxY;
		};

		unsigned int GetCullMask(void){ return cullMask; }
		void SetCullMask(unsigned int m){ cullMask = m; }

		const CameraRect& GetCameraRect(void){ return rect; }
		void SetCameraRect(const CameraRect& r){ rect = r; }

		int GetDepth(void)const{ return depth; }
		void SetDepth(int d){ depth = d; }

		bool TestAabb(const EDMath::Aabb& aabb);

		const EDMath::Aabb& GetFrustumAabb(void)
		{ 
			return GetShadowFrustumAabb(numCascades-1);
		}

		const EDMath::Aabb& GetShadowFrustumAabb(UINT cascadeIndex)
		{ 
			assert(numCascades > cascadeIndex);
			return frustumShadowAabb[cascadeIndex];
		}

		//struct compstruct
		//{
		//	bool operator()(const Camera* a, const Camera* b)const;
		//};

		void ScreenPointToRay( Float3& start, Float3& normal, float x, float y );

		Camera* clone(void)const;

		inline UINT GetNumCascades() { return numCascades; }

	private:

		ILight* visibleLightSet;
		IRenderer* visibleSet;

		unsigned int lightCount;
		unsigned int rendererCount;

		float fieldOfView;
		float nearDistance;
		float farDistance;
		float aspect;
		float nearHeight;
		float nearWidth;
		float farHeight;
		float farWidth;
		int depth;

		CameraRect rect;
		EDMath::Plane frustum[6];
		
		UINT numCascades;
		// One for each cascade...
		EDMath::Aabb *frustumShadowAabb;
		
		Float4x4 projectionMatrix;
		Float4x4 viewMatrix;
		Float4x4 viewProjection;
		Camera* nextCamera;
		unsigned int cullMask;

		const void BuildFrustumPlanes(void);

		set<Camera*, compstruct>* containingSet;
		//set<Camera*>* containingSet;

		void SetLOD(IRenderer* renderable, float maxExtent, float nearDistance,
			float depth, float minArea);
		void SetLODSolution(IRenderer* renderable, float maxExtent, float nearDistance, 
			float depth, float minArea);
	};
}