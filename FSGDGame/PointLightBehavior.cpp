#include "PointLightBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDGOCA/GameObject.h"
#include "../EDCollision/CollisionLibrary.h"

#include "../EDRendererD3D/RenderModel.h"
#include "../EDRendererD3D/RenderShape.h"

#include "RenderController.h"
#include "../EDRendererD3D/DebugRenderer.h"

using namespace EDRendererD3D;
using namespace EDMath;
using namespace EDGOCA;
using namespace EDCollision;
using namespace DirectX;

namespace PointLightBehaviorMessages
{
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

#include "AttributeReaders.h"

namespace PointLightBehaviorAttributes
{
	DeclareAttribute( XMFLOAT4, POINT_LIGHT_DIFFUSE );
	DeclareAttribute( XMFLOAT3, POINT_LIGHT_SPECULAR );
	DeclareAttribute( XMFLOAT3, POINT_LIGHT_AMBIENT );
	DeclareAttribute( XMFLOAT3, POINT_LIGHT_ATTENUATION );
	DeclareAttribute( float, POINT_LIGHT_SPECULAR_INTENSITY );
	DeclareAttribute( float, POINT_LIGHT_SPECULAR_POWER );
}

namespace Behaviors
{
	PointLightBehavior::PointLightBehavior(void)
	{
		lightPtr = new PointLightWithShadow();
		lightPtr->LoadModel( "GDModel/PointLight/mdl_PointLight.xml" );
		//lightPtr->SetName("pointLight0");
		lightPtr->SetDiffuseColor(&XMFLOAT4(1, 1, 1, 1));
		lightPtr->SetSpecularColor(&XMFLOAT3(1, 1, 1));
		lightPtr->SetAmbientColor(&XMFLOAT3(0.1f, 0.1f, 0.1f));
		lightPtr->SetEnabled(true);
		lightPtr->SetAttenuation(&XMFLOAT3(0.0f, 0.15f, 0.035f), 5.0f);
		lightPtr->SetSpecularIntensity(1.0f);
		lightPtr->SetSpecularPower(66.0f);
		/// Change this to allow all point lights to share the same shadow map
		lightPtr->CreateRenderTarget(RenderController::GetPointShadowSize());
	}

	void PointLightBehavior::OnAdd(void)
	{
		this->gameObject->BuildLocalAabb();
	}

	PointLightBehavior::~PointLightBehavior(void)
	{
		delete lightPtr;
	}

	void PointLightBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( PointLightBehaviorMessages::MSG_FRUSTUM_TEST, OnFrustumTest );
		AddMessageHandler( PointLightBehaviorMessages::MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb );
		AddMessageHandler( PointLightBehaviorMessages::MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount );
		AddMessageHandler( GameObjectMessages::MSG_ATTRIBUTE_CHANGED, OnAttributeChanged );
	}

	const AttributeNameKey* PointLightBehavior::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			PointLightBehaviorAttributes::POINT_LIGHT_DIFFUSE,
			PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR,
			PointLightBehaviorAttributes::POINT_LIGHT_AMBIENT,
			PointLightBehaviorAttributes::POINT_LIGHT_ATTENUATION,
			PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR_INTENSITY,
			PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR_POWER,
			0
		};

		return attribs;
	}

	void PointLightBehavior::OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
		message->value += 1;
	}

	void PointLightBehavior::OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		PointLightBehavior* lightBehaviorPtr = (PointLightBehavior*)pInvokingBehavior;

		if( lightBehaviorPtr->lightPtr == 0 )
			return;

		MessageT<FrustumHull*>* message = (MessageT<FrustumHull*>*)msg;

		Aabb tempAabb = lightBehaviorPtr->gameObject->GetAabb();
		if( message->value->Collides( &tempAabb ) )
		{
			// Maintain scale of the light geometry
			Float3 pos = lightBehaviorPtr->gameObject->GetWorldTransform().WAxis;
			XMFLOAT4X4 newMat = *lightBehaviorPtr->lightPtr->GetWorldMatrixPtr();
			newMat._41 = pos.x;
			newMat._42 = pos.y;
			newMat._43 = pos.z;
			newMat._11 = newMat._22 = newMat._33 = lightBehaviorPtr->lightPtr->GetRange();
			lightBehaviorPtr->lightPtr->SetWorldMatrixPtr(&newMat);

			RenderController::GetInstance()->AddVisibleLight(lightBehaviorPtr->lightPtr);
		}
	}

	void PointLightBehavior::OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		PointLightBehavior* lightBehaviorPtr = (PointLightBehavior*)pInvokingBehavior;
		MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

		// Determine the size of the aabb, including scaling of the mesh to be used for rendering
		Aabb localAabb = 
			lightBehaviorPtr->lightPtr->GetRenderModel()->GetShapes()[0]->GetRenderMesh()->GetAabb();
		const XMFLOAT4X4 *lightMat = lightBehaviorPtr->lightPtr->GetWorldMatrixPtr();
		Float3 scale(lightMat->_11, lightMat->_22, lightMat->_33);
		localAabb.max = localAabb.max * (scale * 0.5f);
		localAabb.min = localAabb.min * (scale * 0.5f);

		for(unsigned int i = 0; i < 3; ++i)
		{
			if( message->value->min.v[i] > localAabb.min.v[i] )
				message->value->min.v[i] = localAabb.min.v[i];
			if( message->value->max.v[i] < localAabb.max.v[i] )
				message->value->max.v[i] = localAabb.max.v[i];
		}
	}

	void PointLightBehavior::OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		PointLightBehavior* lightBehaviorPtr = (PointLightBehavior*)pInvokingBehavior;
		MessageT<AttributeNameKey>* message = (MessageT<AttributeNameKey>*)msg;

		if( message->value == PointLightBehaviorAttributes::POINT_LIGHT_DIFFUSE )
		{
			Attribute<XMFLOAT4>* attrib = (Attribute<XMFLOAT4>*)lightBehaviorPtr->GetAttribute( 
				PointLightBehaviorAttributes::POINT_LIGHT_DIFFUSE );
			lightBehaviorPtr->lightPtr->SetDiffuseColor( &attrib->value );
		}
		else if( message->value == PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR )
		{
			Attribute<XMFLOAT3>* attrib = (Attribute<XMFLOAT3>*)lightBehaviorPtr->GetAttribute( 
				PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR );
			lightBehaviorPtr->lightPtr->SetSpecularColor( &attrib->value );
		}
		else if( message->value == PointLightBehaviorAttributes::POINT_LIGHT_AMBIENT )
		{
			Attribute<XMFLOAT3>* attrib = (Attribute<XMFLOAT3>*)lightBehaviorPtr->GetAttribute( 
				PointLightBehaviorAttributes::POINT_LIGHT_AMBIENT );
			lightBehaviorPtr->lightPtr->SetAmbientColor( &attrib->value );
		}
		else if( message->value == PointLightBehaviorAttributes::POINT_LIGHT_ATTENUATION )
		{
			Attribute<XMFLOAT4>* attrib = (Attribute<XMFLOAT4>*)lightBehaviorPtr->GetAttribute( 
				PointLightBehaviorAttributes::POINT_LIGHT_ATTENUATION );
			lightBehaviorPtr->lightPtr->SetAttenuation( (XMFLOAT3 *)&attrib->value, attrib->value.w );
		}
		else if( message->value == PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR_INTENSITY )
		{
			Attribute<float>* attrib = (Attribute<float>*)lightBehaviorPtr->GetAttribute( 
				PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR_INTENSITY );
			lightBehaviorPtr->lightPtr->SetSpecularIntensity( attrib->value );	
		}
		else if( message->value == PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR_POWER )
		{
			Attribute<float>* attrib = (Attribute<float>*)lightBehaviorPtr->GetAttribute( 
				PointLightBehaviorAttributes::POINT_LIGHT_SPECULAR_POWER );
			lightBehaviorPtr->lightPtr->SetSpecularPower( attrib->value );
		}
	}
}
