#include "Transform.h"
#include "Game.h"
#include "GameObject.h"
#include "../tinyxml/tinyxml.h"

namespace EDGameCore
{
	Transform::Transform(void) : parent(0), firstChild(0), nextSibling(0), dirtied(true)
	{
		localMatrix = worldMatrix.makeIdentity();
	}

	Transform::Transform(const Transform& rhs)
	{
		parent = 0;
		firstChild = 0;
		nextSibling = 0;
		dirtied = modified = false;
		localMatrix.makeIdentity();
		worldMatrix.makeIdentity();

		Transform* current = rhs.firstChild;

		for(; current != 0; current = current->nextSibling )
		{
			GameObject* newGameObj = current->GetGameObject()->clone();
			newGameObj->GetTransform()->SetParent( this );
		}

		localMatrix = rhs.localMatrix;
		worldMatrix = rhs.worldMatrix;

		DirtyTransform();
	}

	void Transform::OnDestroy(void)
	{
		Transform* current = firstChild;

		for(; current != 0; current = current->nextSibling)
			Game::destroy( current->GetGameObject() );
	}

	const Float4x4& Transform::GetWorldMatrix(void)const
	{
		return const_cast< Transform* >( this )->_GetWorldMatrix();
	}

	const Float4x4& Transform::_GetWorldMatrix(void)
	{
		// Implement solution for sorting & hierarchy lab
		return _GetWorldMatrixSolution();
	}

	const Float4x4& Transform::GetLocalMatrix(void)const
	{
		return localMatrix;
	}

	void Transform::SetLocalMatrix(const Float4x4& matrix)
	{
		localMatrix = matrix;

		DirtyTransform();
	}

	Transform* Transform::GetParent(void) 
	{ 
		return parent;
	}

	const Transform* Transform::GetParent(void)const
	{ 
		return parent;
	}

	void Transform::SetParent(Transform* newParent)
	{
		if( newParent == this )
			return;

		if( parent == newParent )
			return;

		if( parent != 0 )
			parent->RemoveChild(this);
		
		parent = newParent;

		if( newParent != 0 )
		{
			nextSibling = newParent->firstChild;
			newParent->firstChild = this;

			const Float4x4& world = newParent->GetWorldMatrix();
			Float4x4 invWorld = world;

			invWorld.Wx = -DotProduct(world.XAxis, world.WAxis);
			invWorld.Wy = -DotProduct(world.YAxis, world.WAxis);
			invWorld.Wz = -DotProduct(world.ZAxis, world.WAxis);
			invWorld.transpose3x3();

			Float4x4 newLocal;
			Multiply( newLocal, GetWorldMatrix(), invWorld );
			SetLocalMatrix(newLocal);
		}
	}

	void Transform::RemoveChild(Transform* child)
	{
		if( child == 0 || child->parent != this )
			return;
		
		Transform* prev = 0;
		Transform* current = firstChild;
		
		while(current != child)
		{
			prev = current;
			current = current->nextSibling;
		}

		if( prev == 0 )
			firstChild = current->nextSibling;
		else
			prev->nextSibling = current->nextSibling;

		child->localMatrix = child->worldMatrix;
		child->parent = 0;		

		current->nextSibling = 0;
		current->parent = 0;
		DirtyTransform();
	}

	void Transform::DirtyTransform(void)
	{
		modified = true;

		if( !dirtied )
		{
			dirtied = true;

			Transform* current = firstChild;
			while( current != 0 )
			{
				current->DirtyTransform();
				current = current->nextSibling;
			}
		}
	}

	void Transform::TranslateLocal(const Float3& translation)
	{
		// TODO - comment this out and write your own solution
		TranslateLocalSolution(translation);
		DirtyTransform();
	}

	void Transform::TranslateGlobal(const Float3& translation)
	{
		// TODO - comment this out and write your own solution
		TranslateGlobalSolution(translation);
		DirtyTransform();
	}

	void Transform::RotateLocalX(float r)
	{
		localMatrix.rotateLocalX(r);
		DirtyTransform();
	}

	void Transform::RotateLocalY(float r)
	{
		localMatrix.rotateLocalY(r);
		DirtyTransform();
	}

	void Transform::RotateLocalZ(float r)
	{
		localMatrix.rotateLocalZ(r);
		DirtyTransform();
	}

	void Transform::RotateGlobalX(float r)
	{
		localMatrix.rotateGlobalX(r);
		DirtyTransform();
	}

	void Transform::RotateGlobalY(float r)
	{
		localMatrix.rotateGlobalY(r);
		DirtyTransform();
	}

	void Transform::RotateGlobalZ(float r)
	{
		localMatrix.rotateGlobalZ(r);
		DirtyTransform();
	}

	bool Transform::wasModified(void)
	{
		bool result = modified;
		modified = false;
		return result;
	}

	void Transform::LoadState(TiXmlElement* xmlElement)
	{
		const char* axes[] = {"XAxis", "YAxis", "ZAxis", "WAxis"};
		const char* components[] = {"x", "y", "z", "w"};

		for(int i = 0; i < 4; ++i)
		{
			TiXmlElement* axisEle = xmlElement->FirstChildElement(axes[i]);

			if( axisEle == 0 )
				continue;

			for(int j = 0; j < 4; ++j)
			{
				TiXmlElement* compEle = axisEle->FirstChildElement(components[j]);
				
				if(compEle != 0 &&
					compEle->GetText() != 0 &&
					strlen( compEle->GetText() ) != 0 )
				{
					localMatrix.m[i][j] = (float)atof(compEle->GetText());
				}
			}
		}

		DirtyTransform();
	}

	GameObject* Transform::Find(RegistrationId nameHash)
	{
		GameObject* gameObject = GetGameObject();
		if( gameObject->GetNameHash() == nameHash )
			return gameObject;

		for(Transform* child = firstChild; child != 0; child = child->nextSibling)
		{
			GameObject* childObject = child->GetGameObject();
			RegistrationId childNameHash = childObject->GetNameHash();

			if( childNameHash == nameHash )
				return childObject;
		}

		return 0;		
	}

	GameObject* Transform::Find(const char* name)
	{
		if(name == 0)
			return 0;

		char string[64];
		strcpy_s( string, name );
		char seps[] = "/";
		char* token = 0;
		char* next_token = 0;

		token = strtok_s( string, seps, &next_token );
		
		if(token == 0)
			return 0;

		RegistrationId nameHash = (unsigned int)std::hash<std::string>()(token);
		
		GameObject* gameObject = Find(nameHash);

		if( gameObject == 0 )
			return 0;

		token = strtok_s( 0, seps, &next_token );
		
		while( token != 0 )
		{
			nameHash = (unsigned int)std::hash<std::string>()(token);

			gameObject = gameObject->GetTransform()->Find( nameHash );

			if( gameObject == 0 )
				return 0;

			token = strtok_s( 0, seps, &next_token );
		}

		return gameObject;
	}

	GameObject* Transform::FindInHierarchy(const char* name)
	{
		RegistrationId nameHash = (unsigned int)std::hash<std::string>()(name);

		return FindInHierarchy(nameHash);	
	}

	GameObject* Transform::FindInHierarchy(RegistrationId nameHash)
	{
		GameObject* gameObject = GetGameObject();
		if( gameObject->GetNameHash() == nameHash )
			return gameObject;

		for(Transform* child = firstChild; child != 0; child = child->nextSibling)
		{
			GameObject* result = child->GetGameObject()->GetTransform()->FindInHierarchy(nameHash);

			if( result != 0 )
				return result;
		}

		return 0;		
	}
}