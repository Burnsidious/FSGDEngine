#include "RigidBody.h"
#include "Transform.h"
#include "../tinyxml/tinyxml.h"


namespace EDGameCore
{
	RigidBody::RigidBody(void) : kinematic(false), sleepDelayCounter(0) {}

	RigidBody::RigidBody(const RigidBody& rhs) : ComponentT<RigidBody>(rhs)
	{
		rigidBody = rhs.rigidBody;
		kinematic = rhs.kinematic;
		sleepDelayCounter = 0;
	}

	RigidBody::~RigidBody()
	{
		GameObject* gameObject = GetGameObject();
		if( gameObject && gameObject->GetRigidBody() == this )
			gameObject->SetRigidBody((RigidBody*)nullptr);
	}

	void RigidBody::Update(void)
	{
		GameObject* gameObject = GetGameObject();
		if( !IsKinematic() )
		{
			rigidBody.CenterAt(gameObject->GetTransform()->GetWorldMatrix());
			rigidBody.Update();

			Float4x4 xform;
			rigidBody.GetObjectTransform(xform);

			gameObject->GetTransform()->SetLocalMatrix(xform);
		}
		else
		{
			rigidBody.MoveTo(gameObject->GetTransform()->GetWorldMatrix());
		}
	}

	void RigidBody::CenterAt(const Float4x4& xform)
	{
		Float4x4 newXform = xform;
		
		rigidBody.CenterAt(newXform);

		rigidBody.GetObjectTransform(newXform);

		GetGameObject()->GetTransform()->SetLocalMatrix(newXform);
	}

	void RigidBody::ResolveCollision(RigidBody* lhs, RigidBody* rhs, EDCollision::Contact& state, EDPhysics::MaterialId lhsMaterialId, EDPhysics::MaterialId rhsMaterialId)
	{
		EDPhysics::RigidBodyPhysics::resolveCollision( lhs->rigidBody, rhs->rigidBody, state, lhsMaterialId, rhsMaterialId );

		Float4x4 xform;
		lhs->rigidBody.GetObjectTransform( xform );
		lhs->GetGameObject()->GetTransform()->SetLocalMatrix(xform);
		rhs->rigidBody.GetObjectTransform( xform );
		rhs->GetGameObject()->GetTransform()->SetLocalMatrix(xform);
	}

	void RigidBody::ResolveCollision(RigidBody* lhs, EDCollision::Contact& state, EDPhysics::MaterialId lhsMaterialId, EDPhysics::MaterialId rhsMaterialId)
	{
		EDPhysics::RigidBodyPhysics::resolveCollision( lhs->rigidBody, state, lhsMaterialId, rhsMaterialId );

		Float4x4 xform;
		lhs->rigidBody.GetObjectTransform( xform );
		lhs->GetGameObject()->GetTransform()->SetLocalMatrix(xform);
	}

	void RigidBody::ResolveTireCollision(RigidBody* rigidBody, EDCollision::Contact& contact, float suspensionDistance, float radius, float& motorTorque, const Float4x4& tireMatrix)
	{
		rigidBody->Wake();

		Float3 point = tireMatrix.WAxis - tireMatrix.YAxis * radius;

		EDPhysics::RigidBodyPhysics::ContactPointData contactData;
		rigidBody->rigidBody.getContactPointData( contactData, point );

		float s = contact.PenetrationDepth <= suspensionDistance ? contact.PenetrationDepth : suspensionDistance;

		Float3 opposingForce(0.0f, 0.0f, 0.0f);

		Float4x4 identity;
		identity.makeIdentity();

		contactData.velocity -= tireMatrix.ZAxis * motorTorque;

		EDPhysics::calculateReaction( opposingForce, contactData.velocity, contact.Normal, EDPhysics::MATERIAL_DEFAULT, EDPhysics::MATERIAL_DEFAULT, tireMatrix, identity );

		Float3 PF = tireMatrix.ZAxis * DotProduct( tireMatrix.ZAxis, opposingForce );
		if( DotProduct( tireMatrix.ZAxis * -motorTorque, PF ) > 0.0f )
			opposingForce -= PF;

		contact.PenetrationDepth -= s;
		Float3 displacement = contact.Normal * contact.PenetrationDepth;
			
		rigidBody->rigidBody.applyReaction( rigidBody->rigidBody, displacement, opposingForce, contactData );
		rigidBody->ApplyForceAtPoint( (contact.Normal * s) / EDPhysics::TIME_STEP, point );

		Float4x4 xform;
		rigidBody->rigidBody.GetObjectTransform( xform );
		rigidBody->GetGameObject()->GetTransform()->SetLocalMatrix(xform);
	}

	void RigidBody::ResolveCharacterCollision(RigidBody* rigidBody, EDCollision::Contact& contact, float standY, float stopY, EDPhysics::MaterialId lhsMaterialId, EDPhysics::MaterialId rhsMaterialId )
	{
		Float3 normal = contact.Normal;
		float penetrationDepth = contact.PenetrationDepth;

		if( normal.y > standY )
		{
			normal = Float3(0.0f, 1.0f, 0.0f);
			penetrationDepth = contact.PenetrationDepth;
		}
		else if( normal.y < stopY && normal.y > 0.0f )
		{
			normal.y = 0.0f;
			normal.normalize();
			penetrationDepth = contact.PenetrationDepth * DotProduct( normal, contact.Normal );
		}

		contact.Normal = normal;
		contact.PenetrationDepth = penetrationDepth;
		
		rigidBody->Wake();
		EDPhysics::RigidBodyPhysics::ContactPointData contactData;
		rigidBody->rigidBody.getContactPointData( contactData, contact.Point[0] );

		Float3 opposingForce(0.0f, 0.0f, 0.0f);

		Float4x4 identity;
		identity.makeIdentity();

		EDPhysics::calculateReaction( opposingForce, contactData.velocity, contact.Normal, lhsMaterialId, rhsMaterialId, rigidBody->GetTransform()->GetWorldMatrix(), identity );

		Float3 displacement = contact.Normal * contact.PenetrationDepth;
			
		rigidBody->rigidBody.applyReaction( rigidBody->rigidBody, displacement, opposingForce, contactData );

		Float4x4 xform;
		rigidBody->rigidBody.GetObjectTransform( xform );
		rigidBody->GetGameObject()->GetTransform()->SetLocalMatrix(xform);
	}

	float RigidBody::GetInverseMass(void)
	{
		return rigidBody.GetInverseMass();
	}

	void RigidBody::SetInverseMass(float m)
	{
		rigidBody.SetInverseMass(m);
	}

	void RigidBody::SetKinematic(bool k)
	{ 
		kinematic = k;
	}

	void RigidBody::SetUseGravity(bool g)
	{
		rigidBody.SetGravityState(g);
	}

	void RigidBody::Sleep(void)
	{
		rigidBody.Sleep();
	}

	void RigidBody::Wake(void)
	{
		rigidBody.Wake();
	}

	bool RigidBody::UsesGravity(void)
	{
		return rigidBody.GetGravityState();
	}

	bool RigidBody::IsKinematic(void)
	{ 
		return kinematic;
	}

	bool RigidBody::IsSleeping(void)
	{
		return !rigidBody.IsAwake();
	}

	void RigidBody::SetCenterOfMass(const Float3& c)
	{
		rigidBody.SetCenterOfMass(c);
	}

	const Float3& RigidBody::GetCenterOfMass(void)
	{
		return rigidBody.GetCenterOfMass();
	}

	void RigidBody::GetObjectTransform(Float4x4& xform)const
	{
		rigidBody.GetObjectTransform(xform);
	}

	void RigidBody::GetVelocityAtPoint(const Float3& p, Float3& v)
	{
		return rigidBody.GetVelocityAtPoint(p, v);
	}

	const Float3& RigidBody::GetLinearFactors(void)const
	{
		return rigidBody.GetLinearFactors();
	}

	float RigidBody::GetAngularFactor(void)const
	{
		return rigidBody.GetAngularFactor();
	}

	void RigidBody::SetLinearFactors(const Float3& l)
	{
		rigidBody.SetLinearFactors(l);
	}

	void RigidBody::SetAngularFactor(float a)
	{
		rigidBody.SetAngularFactor(a);
	}

	void RigidBody::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* kinematicEle = xmlElement->FirstChildElement("Kinematic");

		if( kinematicEle )
		{
			kinematic = true;

			if( kinematicEle->GetText() != 0 &&
				strlen(kinematicEle->GetText()) != 0 )
			{
				if( _stricmp( kinematicEle->GetText(), "false" ) == 0 ||
					_stricmp( kinematicEle->GetText(), "0" ) == 0 )
					kinematic = false;
			}
		}

		TiXmlElement* comEle = xmlElement->FirstChildElement("CenterOfMass");

		if( comEle )
		{
			const char* components[] = {"x", "y", "z"};

			Float3 com = ZERO_VECTOR;
			for(int j = 0; j < 3; ++j)
			{
				TiXmlElement* compEle = comEle->FirstChildElement(components[j]);
				
				if(compEle != 0 &&
					compEle->GetText() != 0 &&
					strlen( compEle->GetText() ) != 0 )
				{
					com.v[j] = (float)atof(compEle->GetText());
				}
			}

			rigidBody.SetCenterOfMass(com);
		}

		TiXmlElement* massEle = xmlElement->FirstChildElement("Mass");
		if( massEle )
		{
			if( massEle->GetText() != 0 &&
				strlen( massEle->GetText() ) != 0 )
			{
				float val = (float)atof(massEle->GetText() );

				if( val > 0.0f )
					SetInverseMass(1.0f / val);
				else
					SetInverseMass(0.0f);
			}
		}

		TiXmlElement* sleepEle = xmlElement->FirstChildElement("Sleep");
		if( sleepEle )
		{
			bool putToSleep = true;

			if( sleepEle->GetText() != 0 &&
				strlen(sleepEle->GetText()) != 0 )
			{
				if( _stricmp( sleepEle->GetText(), "false" ) == 0 ||
					_stricmp( sleepEle->GetText(), "0" ) == 0 )
					putToSleep = false;
			}

			if( putToSleep )
				Sleep();
		}

		TiXmlElement* linearEle = xmlElement->FirstChildElement("LinearFactors");
		if( linearEle )
		{
			const char* components[] = {"x", "y", "z"};
			Float3 linears(1.0f, 1.0f, 1.0f);
			for(int j = 0; j < 3; ++j)
			{
				TiXmlElement* compEle = comEle->FirstChildElement(components[j]);
				
				if(compEle != 0 &&
					compEle->GetText() != 0 &&
					strlen( compEle->GetText() ) != 0 )
				{
					linears.v[j] = (float)atof(compEle->GetText());
				}
			}

			SetLinearFactors( linears );
		}

		TiXmlElement* angularEle = xmlElement->FirstChildElement("AngularFactor");

		if( angularEle &&
			angularEle->GetText() != 0 &&
			strlen( angularEle->GetText() ) != 0 )
			SetAngularFactor( (float)atof( angularEle->GetText() ) );
	}

	void RigidBody::AddForce(const Float3& f)
	{
		rigidBody.ApplyForce(f);
	}

	void RigidBody::ApplyForceAtPoint(const Float3& f, const Float3& p)
	{
		Wake();
		rigidBody.ApplyForceAtPoint(f, p);
	}

	void RigidBody::ApplyDisplacementAtPoint(const Float3& delta, const Float3& p)
	{
		rigidBody.ApplyDisplacementAtPoint( delta, p );

		GameObject* gameObject = GetGameObject();
		rigidBody.CenterAt(gameObject->GetTransform()->GetWorldMatrix());

		Float4x4 xform;
		rigidBody.GetObjectTransform(xform);

		gameObject->GetTransform()->SetLocalMatrix(xform);
	}
}