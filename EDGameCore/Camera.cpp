#include "Camera.h"

#include "../EDUtilities/Settings.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"
#include "Light.h"
#include "LayerManager.h"

namespace EDGameCore
{
	Camera::Camera(void)
	{
		fieldOfView = (float)M_PI * 0.25f;
		nearDistance = 0.1f;
		farDistance = 400.0f;
		aspect = 16.0f / 9.0f;
		nearHeight = 0.0f;
		nearWidth = 0.0f;
		nextCamera = 0;
		cullMask = 0xFFFFFFFF;


		visibleLightSet = 0;
		visibleSet = 0;
		lightCount = 0;
		rendererCount = 0;

		rect.minX = rect.minY = 0.0f;
		rect.maxX = 1.0f;
		rect.maxY = 1.0f;
		depth = 0;

		containingSet = 0;

		frustumShadowAabb = nullptr;
		numCascades = 0;
	}

	Camera::Camera(const Camera& rhs)
	{
		visibleLightSet = 0;
		visibleSet = 0;
		lightCount = 0;
		rendererCount = 0;
		nextCamera = 0;
		containingSet = 0;

		fieldOfView = rhs.fieldOfView;
		nearDistance = rhs.nearDistance;
		farDistance = rhs.farDistance;
		aspect = rhs.aspect;
		nearHeight = rhs.nearHeight;
		nearWidth = rhs.nearWidth;
		farHeight = rhs.farHeight;
		farWidth = rhs.farWidth;
		depth = rhs.depth;

		rect = rhs.rect;
		for(int i = 0; i < 6; ++i)
			frustum[i] = rhs.frustum[i];
		
		numCascades = rhs.numCascades;

		frustumShadowAabb = new EDMath::Aabb[numCascades];

		for (unsigned int i = 0; i < numCascades; ++i)
			frustumShadowAabb[i] = rhs.frustumShadowAabb[i];

		projectionMatrix = rhs.projectionMatrix;
		viewMatrix = rhs.viewMatrix;
		viewProjection = rhs.viewProjection;
		cullMask = rhs.cullMask;
	}

	Camera::~Camera()
	{
		delete[] frustumShadowAabb;

		if( containingSet != 0 )
		{
			containingSet->erase( this );
			containingSet = 0;
		}

		GameObject* gameObject = GetGameObject();
		if( gameObject && gameObject->GetCamera() == this )
			gameObject->SetCamera((Camera*)nullptr);
	}

	void Camera::LoadState(TiXmlElement* xmlElement)
	{
		//fieldOfView = (float)M_PI * 0.40f;
		//nearDistance = 0.1f;
		//farDistance = 150.0f;
		//aspect = 16.0f / 9.0f;

		TiXmlElement *fovElement = xmlElement->FirstChildElement("FieldOfView");
		if (fovElement != 0 && fovElement->GetText() != 0 && strlen(fovElement->GetText()) != 0)
		{
			fieldOfView = (float)atof(fovElement->GetText());
		}

		TiXmlElement *nearDistanceElement= xmlElement->FirstChildElement("NearDistance");
		if (nearDistanceElement != 0 && nearDistanceElement->GetText() != 0 && 
			strlen(nearDistanceElement->GetText()) != 0)
		{
			nearDistance = (float)atof(nearDistanceElement->GetText());
		}

		TiXmlElement *farDistanceElement = xmlElement->FirstChildElement("FarDistance");
		if (farDistanceElement != 0 && farDistanceElement->GetText() != 0 && 
			strlen(farDistanceElement->GetText()) != 0)
		{
			farDistance = (float)atof(farDistanceElement->GetText());
		}

		TiXmlElement *aspectElement = xmlElement->FirstChildElement("AspectRatio");
		if (aspectElement != 0 && aspectElement->GetText() != 0 && strlen(aspectElement->GetText()) != 0)
		{
			aspect = (float)atof(aspectElement->GetText());
		}

	}

	Camera* Camera::clone(void)const
	{
		return new Camera(*this);
	}

	void Camera::Update(void)
	{
		visibleSet = 0;
		visibleLightSet = 0;
		lightCount = 0;
		rendererCount = 0;

		float scale = 2.0f * tanf(0.5f*fieldOfView);
		nearHeight = nearDistance * scale;
		nearWidth = nearHeight * aspect;
		farHeight = farDistance * scale;
		farWidth = farHeight * aspect;

		// Get the number of cascades desired
		EDUtilities::Settings::GetInstance()->GetSetting("NumCascades", numCascades, 3);

		// Create the cascade objects
		delete [] frustumShadowAabb;
		frustumShadowAabb = new EDMath::Aabb[numCascades];

		string CascadePercent = "CascadePercent";

		for (int cascadeIndex = numCascades - 1; cascadeIndex >= 0; --cascadeIndex)
		{
			float cascadeFarPercentage;
			EDUtilities::Settings::GetInstance()->GetSetting(
				(CascadePercent + char('1' + cascadeIndex)).c_str(), cascadeFarPercentage, 1.0);


			float cascadeFar = farDistance * cascadeFarPercentage;
			float cascadeFarHeight = cascadeFar * scale;
			float cascadeFarWidth =  cascadeFarHeight * aspect;

			frustumShadowAabb[cascadeIndex].center = GetGameObject()->GetTransform()->GetWorldMatrix().WAxis +
				GetGameObject()->GetTransform()->GetWorldMatrix().ZAxis *
				(nearDistance + cascadeFar) * 0.5f;

			Float3 ftr = GetGameObject()->GetTransform()->GetWorldMatrix().WAxis +
				GetGameObject()->GetTransform()->GetWorldMatrix().ZAxis * cascadeFar +
				GetGameObject()->GetTransform()->GetWorldMatrix().XAxis * cascadeFarWidth * 0.5f +
				GetGameObject()->GetTransform()->GetWorldMatrix().YAxis * cascadeFarHeight * 0.5f;

			frustumShadowAabb[cascadeIndex].extents.x = (ftr - frustumShadowAabb[cascadeIndex].center).magnitude();
			frustumShadowAabb[cascadeIndex].extents.y = frustumShadowAabb[cascadeIndex].extents.z = 
				frustumShadowAabb[cascadeIndex].extents.x;
		}

		/*float cascadeFar = nearDistance;

		for (int cascadeIndex = numCascades - 1; cascadeIndex >= 0; --cascadeIndex)
		{
			float cascadeFarPercentage;
			EDUtilities::Settings::GetInstance()->GetSetting(
				(CascadePercent + char('1' + cascadeIndex)).c_str(), cascadeFarPercentage, 1.0);

			float cascadeNear = cascadeFar;
			cascadeFar = farDistance * cascadeFarPercentage;
			float cascadeFarHeight = cascadeFar * scale;
			float cascadeFarWidth = cascadeFarHeight * aspect;

			frustumShadowAabb[cascadeIndex].center = GetGameObject()->GetTransform()->GetWorldMatrix().WAxis +
				GetGameObject()->GetTransform()->GetWorldMatrix().ZAxis *
				(cascadeNear + cascadeFar) * 0.5f;

			Float3 ftr = GetGameObject()->GetTransform()->GetWorldMatrix().WAxis +
				GetGameObject()->GetTransform()->GetWorldMatrix().ZAxis * cascadeFar +
				GetGameObject()->GetTransform()->GetWorldMatrix().XAxis * cascadeFarWidth * 0.5f +
				GetGameObject()->GetTransform()->GetWorldMatrix().YAxis * cascadeFarHeight * 0.5f;

			frustumShadowAabb[cascadeIndex].extents.x = (ftr - frustumShadowAabb[cascadeIndex].center).magnitude();
			frustumShadowAabb[cascadeIndex].extents.y = frustumShadowAabb[cascadeIndex].extents.z =
				frustumShadowAabb[cascadeIndex].extents.x;
		}*/

		BuildFrustumPlanes();
	}

	void Camera::SetNearDistance(float n)
	{
		nearDistance = n;
	}

	void Camera::SetFarDistance(float f)
	{
		farDistance = f;
	}

	const Float4x4& Camera::GetProjectionMatrix(void)
	{
		projectionMatrix.makePerspectiveLH( fieldOfView, aspect, nearDistance, farDistance );	

		return projectionMatrix;
	}

	const Float4x4& Camera::GetViewMatrix(void)
	{
		// TODO - comment this out and write your own solution
		return GetViewMatrixSolution();
	}

	const Float4x4& Camera::GetViewProjectionMatrix(void)
	{
		Multiply( viewProjection, GetViewMatrix(), GetProjectionMatrix() );

		return viewProjection;
	}

	const void Camera::BuildFrustumPlanes(void)
	{
		const Float4x4& vp = GetViewProjectionMatrix();

		/*OPENGL*/
		//frustum[0] = ComputePlane( vp.ma[3] + vp.ma[2], vp.ma[7] + vp.ma[6], vp.ma[11] + vp.ma[10], -(vp.ma[15] + vp.ma[14]) );
		//frustum[1] = ComputePlane( vp.ma[3] - vp.ma[2], vp.ma[7] - vp.ma[6], vp.ma[11] - vp.ma[10], -(vp.ma[15] - vp.ma[14]) );
		//frustum[2] = ComputePlane( vp.ma[3] + vp.ma[0], vp.ma[7] + vp.ma[4], vp.ma[11] + vp.ma[8], -(vp.ma[15] + vp.ma[12]) );
		//frustum[3] = ComputePlane( vp.ma[3] - vp.ma[0], vp.ma[7] - vp.ma[4], vp.ma[11] - vp.ma[8], -(vp.ma[15] - vp.ma[12]) );
		//frustum[4] = ComputePlane( vp.ma[3] - vp.ma[1], vp.ma[7] - vp.ma[5], vp.ma[11] - vp.ma[9], -(vp.ma[15] - vp.ma[13]) );
		//frustum[5] = ComputePlane( vp.ma[3] + vp.ma[1], vp.ma[7] + vp.ma[5], vp.ma[11] + vp.ma[9], -(vp.ma[15] + vp.ma[13]) );		
		
		/*D3D*/
		frustum[0] = ComputePlane( vp.m[0][2], vp.m[1][2], vp.m[2][2], -vp.m[3][2] );
		frustum[1] = ComputePlane( vp.m[0][3] - vp.m[0][2], vp.m[1][3] - vp.m[1][2], vp.m[2][3] - vp.m[2][2], -(vp.m[3][3] - vp.m[3][2]) );
		frustum[2] = ComputePlane( vp.m[0][3] + vp.m[0][0], vp.m[1][3] + vp.m[1][0], vp.m[2][3] + vp.m[2][0], -(vp.m[3][3] + vp.m[3][0]) );
		frustum[3] = ComputePlane( vp.m[0][3] - vp.m[0][0], vp.m[1][3] - vp.m[1][0], vp.m[2][3] - vp.m[2][0], -(vp.m[3][3] - vp.m[3][0]) );
		frustum[4] = ComputePlane( vp.m[0][3] - vp.m[0][1], vp.m[1][3] - vp.m[1][1], vp.m[2][3] - vp.m[2][1], -(vp.m[3][3] - vp.m[3][1]) );
		frustum[5] = ComputePlane( vp.m[0][3] + vp.m[0][1], vp.m[1][3] + vp.m[1][1], vp.m[2][3] + vp.m[2][1], -(vp.m[3][3] + vp.m[3][1]) );
	}

	bool Camera::CullLight(ILight* light)
	{
		if( !light->isEnabledInHierarchy() )
			return false;

		if( (Game::GetLayerMask( light->GetGameObject()->GetLayer() ) & cullMask) == 0 )
			return false;
		
		if(	light->GetLightType() != ILight::DIRECTIONAL )
		{
			IPointLight* pointLight = (IPointLight*)light;
			
			if( !TestAabb( pointLight->GetAabb() ) )
				return false;
		}

		return true;
	}

	bool Camera::CullRenderable(IRenderer* renderable)
	{
		if( !renderable->isEnabledInHierarchy() )
			return false;

		if( (Game::GetLayerMask( renderable->GetGameObject()->GetLayer() ) & cullMask) == 0 )
			return false;

		const Float4x4& worldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix();

		const EDMath::Aabb& aabb = renderable->GetAabb();

		float r = aabb.extents.x * abs(frustum[0].normal.x) + 
				aabb.extents.y * abs(frustum[0].normal.y) +
				aabb.extents.z * abs(frustum[0].normal.z);

		float depth = DotProduct(frustum[0].normal, aabb.center) - frustum[0].distance;

		if( depth < -r )
			return false;

		for(int i = 1; i < 6; ++i)
		{
			float r = aabb.extents.x * abs(frustum[i].normal.x) + 
				aabb.extents.y * abs(frustum[i].normal.y) +
				aabb.extents.z * abs(frustum[i].normal.z);

			float d = DotProduct(frustum[i].normal, aabb.center) - frustum[i].distance;

			if( d < -r )
				return false;
		}

		renderable->SetZDepth( depth - r );

		unsigned int numLODs = renderable->GetNumLODs();

		// This is effectively the current radius of the object to be tested
		float maxExtent = max( aabb.extents.x, max( aabb.extents.y, aabb.extents.z ) );

		const float nearPercentForFullLOD = 0.1f;
		float minArea = nearWidth * nearPercentForFullLOD * nearHeight * nearPercentForFullLOD;

		SetLOD(renderable, maxExtent, nearDistance, depth, minArea);

		return true;
	}

	void Camera::SetLOD(IRenderer* renderable, float maxExtent, float nearDistance, 
			float depth, float minArea)
	{
		// Implement a solution for LOD lab
		SetLODSolution(renderable, maxExtent, nearDistance, depth, minArea);
	}

	bool Camera::TestAabb(const EDMath::Aabb& aabb)
	{
		for(int i = 0; i < 6; ++i)
		{
			float r = aabb.extents.x * abs(frustum[i].normal.x) + 
				aabb.extents.y * abs(frustum[i].normal.y) +
				aabb.extents.z * abs(frustum[i].normal.z);

			float d = DotProduct(frustum[i].normal, aabb.center) - frustum[i].distance;

			if( d < -r )
				return false;
		}

		return true;
	}

	void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2)
	{
		result[0]=matrix1[0]*matrix2[0]+
			matrix1[4]*matrix2[1]+
			matrix1[8]*matrix2[2]+
			matrix1[12]*matrix2[3];
		result[4]=matrix1[0]*matrix2[4]+
			matrix1[4]*matrix2[5]+
			matrix1[8]*matrix2[6]+
			matrix1[12]*matrix2[7];
		result[8]=matrix1[0]*matrix2[8]+
			matrix1[4]*matrix2[9]+
			matrix1[8]*matrix2[10]+
			matrix1[12]*matrix2[11];
		result[12]=matrix1[0]*matrix2[12]+
			matrix1[4]*matrix2[13]+
			matrix1[8]*matrix2[14]+
			matrix1[12]*matrix2[15];
		result[1]=matrix1[1]*matrix2[0]+
			matrix1[5]*matrix2[1]+
			matrix1[9]*matrix2[2]+
			matrix1[13]*matrix2[3];
		result[5]=matrix1[1]*matrix2[4]+
			matrix1[5]*matrix2[5]+
			matrix1[9]*matrix2[6]+
			matrix1[13]*matrix2[7];
		result[9]=matrix1[1]*matrix2[8]+
			matrix1[5]*matrix2[9]+
			matrix1[9]*matrix2[10]+
			matrix1[13]*matrix2[11];
		result[13]=matrix1[1]*matrix2[12]+
			matrix1[5]*matrix2[13]+
			matrix1[9]*matrix2[14]+
			matrix1[13]*matrix2[15];
		result[2]=matrix1[2]*matrix2[0]+
			matrix1[6]*matrix2[1]+
			matrix1[10]*matrix2[2]+
			matrix1[14]*matrix2[3];
		result[6]=matrix1[2]*matrix2[4]+
			matrix1[6]*matrix2[5]+
			matrix1[10]*matrix2[6]+
			matrix1[14]*matrix2[7];
		result[10]=matrix1[2]*matrix2[8]+
			matrix1[6]*matrix2[9]+
			matrix1[10]*matrix2[10]+
			matrix1[14]*matrix2[11];
		result[14]=matrix1[2]*matrix2[12]+
			matrix1[6]*matrix2[13]+
			matrix1[10]*matrix2[14]+
			matrix1[14]*matrix2[15];
		result[3]=matrix1[3]*matrix2[0]+
			matrix1[7]*matrix2[1]+
			matrix1[11]*matrix2[2]+
			matrix1[15]*matrix2[3];
		result[7]=matrix1[3]*matrix2[4]+
			matrix1[7]*matrix2[5]+
			matrix1[11]*matrix2[6]+
			matrix1[15]*matrix2[7];
		result[11]=matrix1[3]*matrix2[8]+
			matrix1[7]*matrix2[9]+
			matrix1[11]*matrix2[10]+
			matrix1[15]*matrix2[11];
		result[15]=matrix1[3]*matrix2[12]+
			matrix1[7]*matrix2[13]+
			matrix1[11]*matrix2[14]+
			matrix1[15]*matrix2[15];
	}
 
	void MultiplyMatrixByVector4by4OpenGL_FLOAT(float *resultvector, const float *matrix, const float *pvector)
	{
		resultvector[0]=matrix[0]*pvector[0]+matrix[4]*pvector[1]+matrix[8]*pvector[2]+matrix[12]*pvector[3];
		resultvector[1]=matrix[1]*pvector[0]+matrix[5]*pvector[1]+matrix[9]*pvector[2]+matrix[13]*pvector[3];
		resultvector[2]=matrix[2]*pvector[0]+matrix[6]*pvector[1]+matrix[10]*pvector[2]+matrix[14]*pvector[3];
		resultvector[3]=matrix[3]*pvector[0]+matrix[7]*pvector[1]+matrix[11]*pvector[2]+matrix[15]*pvector[3];
	}
 
	#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
	#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
	#define MAT(m,r,c) (m)[(c)*4+(r)]
 
	//This code comes directly from GLU except that it is for float
	int glhInvertMatrixf2(float *m, float *out)
	{
		float wtmp[4][8];
		float m0, m1, m2, m3, s;
		float *r0, *r1, *r2, *r3;
		r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
		r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
			r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
			r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
			r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
			r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
			r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
			r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
			r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
			r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
			r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
			r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
			r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
		/* choose pivot - or die */
		if (fabsf(r3[0]) > fabsf(r2[0]))
			SWAP_ROWS_FLOAT(r3, r2);
		if (fabsf(r2[0]) > fabsf(r1[0]))
			SWAP_ROWS_FLOAT(r2, r1);
		if (fabsf(r1[0]) > fabsf(r0[0]))
			SWAP_ROWS_FLOAT(r1, r0);
		if (0.0 == r0[0])
			return 0;
		/* eliminate first variable     */
		m1 = r1[0] / r0[0];
		m2 = r2[0] / r0[0];
		m3 = r3[0] / r0[0];
		s = r0[1];
		r1[1] -= m1 * s;
		r2[1] -= m2 * s;
		r3[1] -= m3 * s;
		s = r0[2];
		r1[2] -= m1 * s;
		r2[2] -= m2 * s;
		r3[2] -= m3 * s;
		s = r0[3];
		r1[3] -= m1 * s;
		r2[3] -= m2 * s;
		r3[3] -= m3 * s;
		s = r0[4];
		if (s != 0.0) {
			r1[4] -= m1 * s;
			r2[4] -= m2 * s;
			r3[4] -= m3 * s;
		}
		s = r0[5];
		if (s != 0.0) {
			r1[5] -= m1 * s;
			r2[5] -= m2 * s;
			r3[5] -= m3 * s;
		}
		s = r0[6];
		if (s != 0.0) {
			r1[6] -= m1 * s;
			r2[6] -= m2 * s;
			r3[6] -= m3 * s;
		}
		s = r0[7];
		if (s != 0.0) {
			r1[7] -= m1 * s;
			r2[7] -= m2 * s;
			r3[7] -= m3 * s;
		}
		/* choose pivot - or die */
		if (fabsf(r3[1]) > fabsf(r2[1]))
			SWAP_ROWS_FLOAT(r3, r2);
		if (fabsf(r2[1]) > fabsf(r1[1]))
			SWAP_ROWS_FLOAT(r2, r1);
		if (0.0 == r1[1])
			return 0;
		/* eliminate second variable */
		m2 = r2[1] / r1[1];
		m3 = r3[1] / r1[1];
		r2[2] -= m2 * r1[2];
		r3[2] -= m3 * r1[2];
		r2[3] -= m2 * r1[3];
		r3[3] -= m3 * r1[3];
		s = r1[4];
		if (0.0 != s) {
			r2[4] -= m2 * s;
			r3[4] -= m3 * s;
		}
		s = r1[5];
		if (0.0 != s) {
			r2[5] -= m2 * s;
			r3[5] -= m3 * s;
		}
		s = r1[6];
		if (0.0 != s) {
			r2[6] -= m2 * s;
			r3[6] -= m3 * s;
		}
		s = r1[7];
		if (0.0 != s) {
			r2[7] -= m2 * s;
			r3[7] -= m3 * s;
		}
		/* choose pivot - or die */
		if (fabsf(r3[2]) > fabsf(r2[2]))
			SWAP_ROWS_FLOAT(r3, r2);
		if (0.0 == r2[2])
			return 0;
		/* eliminate third variable */
		m3 = r3[2] / r2[2];
		r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
			r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];
		/* last check */
		if (0.0 == r3[3])
			return 0;
		s = 1.0f / r3[3];             /* now back substitute row 3 */
		r3[4] *= s;
		r3[5] *= s;
		r3[6] *= s;
		r3[7] *= s;
		m2 = r2[3];                  /* now back substitute row 2 */
		s = 1.0f / r2[2];
		r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
			r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
		m1 = r1[3];
		r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
			r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
		m0 = r0[3];
		r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
			r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
		m1 = r1[2];                  /* now back substitute row 1 */
		s = 1.0f / r1[1];
		r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
			r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
		m0 = r0[2];
		r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
			r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
		m0 = r0[1];                  /* now back substitute row 0 */
		s = 1.0f / r0[0];
		r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
			r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);
		MAT(out, 0, 0) = r0[4];
		MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
		MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
		MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
		MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
		MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
		MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
		MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
		MAT(out, 3, 3) = r3[7];
		return 1;
	}

	int glhUnProjectf(float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate)
	{
		//Transformation matrices
		float m[16], A[16];
		float in[4], out[4];
		//Calculation for inverting a matrix, compute projection x modelview
		//and store in A[16]
		MultiplyMatrices4by4OpenGL_FLOAT(A, projection, modelview);
		//Now compute the inverse of matrix A
		if(glhInvertMatrixf2(A, m)==0)
			return 0;
		//Transformation of normalized coordinates between -1 and 1
		in[0]=(winx-(float)viewport[0])/(float)viewport[2]*2.0f-1.0f;
		in[1]=(winy-(float)viewport[1])/(float)viewport[3]*2.0f-1.0f;
		in[2]=2.0f*winz-1.0f;
		in[3]=1.0f;
		//Objects coordinates
		MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
		if(out[3]==0.0f)
			return 0;
		out[3]=1.0f/out[3];
		objectCoordinate[0]=out[0]*out[3];
		objectCoordinate[1]=out[1]*out[3];
		objectCoordinate[2]=out[2]*out[3];
		return 1;
	}

	void Camera::ScreenPointToRay( Float3& start, Float3& normal, float x, float y )
	{
		Float3 nearPos;
		Float3 farPos;

		Float4x4 viewMat = GetViewMatrix();
		Float4x4 projMat = GetProjectionMatrix();
		Float3 camPos = GetTransform()->GetWorldMatrix().WAxis;

		CameraRect rect = GetCameraRect();
		int viewPort[4] = { int(rect.minX), int(rect.minY), int(rect.maxX - rect.minX), int(rect.maxY - rect.minY) };

		y = viewPort[3] - y;

		glhUnProjectf( x, y, 0.0f, viewMat.ma, projMat.ma, viewPort, nearPos.v );
		//glhUnProjectf( x, y, 1.0f, viewMat.ma, projMat.ma, viewPort, farPos.v );

		start = nearPos;
		//normal = (farPos - nearPos).normalize();
		normal = (nearPos - camPos).normalize();
	}
}