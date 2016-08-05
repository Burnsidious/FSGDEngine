#include "Picking.h"
#include "../EDGameCore/GameObject.h"
#include "../EDGameCore/Camera.h"
#include "../EDGameCore/Collider.h"
#include "../EDGameCore/RigidBody.h"
#include "../EDGameCore/Transform.h"
#include "../EDGameCore/MeshCollider.h"
#include "../EDGameCore/CharacterCollider.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDRendererD3D/DebugRenderer.h"

using EDUtilities::PrintConsole;

void Picking::OnMessage(EDGameCore::Message& message, const void *data)
{
	static const EDGameCore::RegistrationId MSG_SELECT = EDGameCore::Message("MSG_SELECT").messageId;
	static const EDGameCore::RegistrationId MSG_DRAG = EDGameCore::Message("MSG_DRAG").messageId;

	EDGameCore::Camera* cam = GetCamera();

	if( message.messageId == MSG_SELECT )
	{
		manipulatedInstanceId = 0;
		selectedInstanceId = 0;

		POINT pos = *static_cast<const POINT *>(data);

		float lastX = float(pos.x) / EDGameCore::Game::GetWindowWidth();
		float lastY = float(pos.y) / EDGameCore::Game::GetWindowHeight();
		
		Float3 start;
		Float3 normal;
		cam->ScreenPointToRay( start, normal, lastX, lastY );

		if( EDGameCore::Physics::RayCast( start, normal, FLT_MAX, 0xFFFFFFFF, rayHit ) )
		{
			if( rayHit.collider != 0 )
			{
				if( rayHit.collider->GetAttachedRigidBody() != 0 )
				{
					Float3 lastIntersection = rayHit.Point;
					InvTransformPoint( localIntersection, lastIntersection, rayHit.collider->GetAttachedRigidBody()->GetTransform()->GetWorldMatrix() );

					manipulatedInstanceId = rayHit.collider->GetAttachedRigidBody()->GetGameObject()->GetInstanceId();
					selectedInstanceId = manipulatedInstanceId;
				}
				else
				{
					selectedInstanceId = rayHit.collider->GetGameObject()->GetInstanceId();
				}
			}
		}
	}
	else if( message.messageId == MSG_DRAG )
	{
		if( manipulatedInstanceId == 0 )
			return;

		EDGameCore::GameObject* manipulatedObject = EDGameCore::GameObject::GetGameObjectInstance(manipulatedInstanceId);

		if( manipulatedObject == 0 )
		{
			manipulatedInstanceId = 0;
			return;
		}

		POINT pos = *static_cast<const POINT *>(data);
			
		float newX = float(pos.x) / EDGameCore::Game::GetWindowWidth();
		float newY = float(pos.y) / EDGameCore::Game::GetWindowHeight();

		Float3 start;
		Float3 normal;
		cam->ScreenPointToRay( start, normal, newX, newY );

		Float3 lastIntersection;
		TransformPoint( lastIntersection, localIntersection, manipulatedObject->GetTransform()->GetWorldMatrix() );

		float lastDistance = DotProduct( lastIntersection - cam->GetTransform()->GetWorldMatrix().WAxis, cam->GetTransform()->GetWorldMatrix().ZAxis );
		float s = lastDistance / DotProduct( normal, cam->GetTransform()->GetWorldMatrix().ZAxis );

		newPosition = cam->GetTransform()->GetWorldMatrix().WAxis + normal * s;
	}
}

void Picking::OnMessage(EDGameCore::Message& message)
{
	static const EDGameCore::RegistrationId MSG_RELEASE = EDGameCore::Message("MSG_RELEASE").messageId;

	if( message.messageId == MSG_RELEASE )
		manipulatedInstanceId = 0;
}

void Picking::Update(void)
{
	if( selectedInstanceId != 0 )
	{
		EDGameCore::GameObject* selectedObject = EDGameCore::GameObject::GetGameObjectInstance(selectedInstanceId);

		if( selectedObject == 0 )
		{
			selectedInstanceId = 0;
			manipulatedInstanceId = 0;
			return;
		}

		const size_t cSize = strlen(selectedObject->GetName()) + 1;
		wstring name( cSize, L'#' );
		size_t numConverted = 0;
		mbstowcs_s( &numConverted, &name[0], cSize, selectedObject->GetName(), cSize );

		PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.0f, 0.0f, 0.0f), L"Name: %s", name.c_str() );
		PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.0f, 0.0f, 0.0f), L"InstanceId: %u", selectedObject->GetInstanceId() );

		if( rayHit.triIndex != EDGameCore::INVALID_TRI_INDEX )
		{
			PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.0f, 0.0f, 0.0f), L"TriIndex: %d", rayHit.triIndex );
			PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection,
													Float3(0.0f, 0.0f, 0.0f),
													L"Barycentric Coords: ( %f, %f, %f ) ",
													rayHit.Barycentric.x,
													rayHit.Barycentric.y,
													rayHit.Barycentric.z);

			Float3 verts[3];
			
			EDGameCore::IMeshCollider* meshCollider = (EDGameCore::IMeshCollider*)rayHit.collider;
			
			meshCollider->GetPoints( rayHit.triIndex, verts[0], verts[1], verts[2] );

			TransformPoint( verts[0], verts[0], meshCollider->GetTransform()->GetWorldMatrix() );
			TransformPoint( verts[1], verts[1], meshCollider->GetTransform()->GetWorldMatrix() );
			TransformPoint( verts[2], verts[2], meshCollider->GetTransform()->GetWorldMatrix() );

			Float3 start = verts[2];
			EDRendererD3D::VERTEX_POSCOLOR a;
			EDRendererD3D::VERTEX_POSCOLOR b;

			a.color = b.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			for(unsigned int i = 0; i < 3; ++i)
			{
				a.position = XMFLOAT3(start.v);
				b.position = XMFLOAT3(verts[i].v);
				start = verts[i];

				EDRendererD3D::DebugRenderer::GetInstance()->DrawSegment( a, b );
			}

			EDRendererD3D::DebugRenderer::GetInstance()->DrawSphere( rayHit.Point, 0.1f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) );
		}
		else
		{
			if( rayHit.collider->GetColliderType() == EDGameCore::CAPSULE )
			{
				EDGameCore::CapsuleCollider* capsuleCollider = (EDGameCore::CapsuleCollider*)rayHit.collider;
				
				EDRendererD3D::DebugRenderer::GetInstance()->DrawCapsule( capsuleCollider->GetWorldCapsule() );
			}
			else if( rayHit.collider->GetColliderType() == EDGameCore::CHARACTER )
			{
				EDGameCore::CharacterCollider* charCollider = (EDGameCore::CharacterCollider*)rayHit.collider;
				
				EDRendererD3D::DebugRenderer::GetInstance()->DrawCapsule( charCollider->GetWorldCapsule() );
			}
			else if( rayHit.collider->GetColliderType() == EDGameCore::SPHERE )
			{
				EDGameCore::SphereCollider* sphereCollider = (EDGameCore::SphereCollider*)rayHit.collider;
				
				EDRendererD3D::DebugRenderer::GetInstance()->DrawSphere( sphereCollider->GetWorldSphere() );				
			}
			else if( rayHit.collider->GetColliderType() == EDGameCore::BOX )
			{
				EDGameCore::BoxCollider* boxCollider = (EDGameCore::BoxCollider*)rayHit.collider;
				
				EDRendererD3D::DebugRenderer::GetInstance()->DrawObb( boxCollider->GetWorldBox() );				
			}
		}

		auto func = [](Component* component)
		{
			const size_t cSize = strlen(component->GetTypeName()) + 1;
			wstring name( cSize, L'#' );
			size_t numConverted = 0;
			mbstowcs_s( &numConverted, &name[0], cSize, component->GetTypeName(), cSize );

			PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.75f, 0.0f, 0.0f), L"    %s", name.c_str() );
		};

		PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.0f, 0.0f, 0.0f), L"Components:");
		selectedObject->ForEachComponent(func);

		auto func2 = [](IBehavior* behavior)
		{
			const size_t cSize = strlen(behavior->GetTypeName()) + 1;
			wstring name( cSize, L'#' );
			size_t numConverted = 0;
			mbstowcs_s( &numConverted, &name[0], cSize, behavior->GetTypeName(), cSize );

			PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.0f, 0.0f, 0.75f), L"    %s", name.c_str() );
		};

		PrintConsole::GetReference().AddString(PrintConsole::PPN_Selection, Float3(0.0f, 0.0f, 0.0f), L"Behaviors:");
		selectedObject->ForEachBehavior(func2);
	}
}

void Picking::FixedUpdate(void)
{
	if( manipulatedInstanceId != 0 )
	{
		EDGameCore::GameObject* manipulatedObject = EDGameCore::GameObject::GetGameObjectInstance(manipulatedInstanceId);
		
		if( manipulatedObject == 0 )
		{
			manipulatedInstanceId = 0;
			return;
		}

		Float3 lastIntersection;
		TransformPoint( lastIntersection, localIntersection, manipulatedObject->GetTransform()->GetWorldMatrix() );

		Float3 delta = newPosition - lastIntersection;
		
		manipulatedObject->GetTransform()->TranslateGlobal(delta);
	}
}
