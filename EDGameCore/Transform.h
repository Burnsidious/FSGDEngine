#pragma once

#include "Component.h"
#include <list>
#include <queue>
#include <stack>
#include "../EDMath/float4x4.h"
using namespace EDMath;

using namespace std;


namespace EDGameCore
{
	class GameObject;

	class Transform : public ComponentT< Transform >
	{
		friend class GameObject;
	public:
		Transform(void);
		Transform(const Transform& rhs);

		~Transform(){}

		ComponentType GetComponentType(void)const{ return TRANSFORM; }

		const Float4x4& GetWorldMatrix(void)const;

		const Float4x4& GetLocalMatrix(void)const;
		void SetLocalMatrix(const Float4x4& matrix);

		Transform* GetParent(void);
		const Transform* GetParent(void)const;

		void SetParent(Transform* newParent);

		void TranslateLocal(const Float3& translation);
		void TranslateLocalSolution(const Float3& translation);
		void TranslateGlobal(const Float3& translation);
		void TranslateGlobalSolution(const Float3& translation);

		void RotateLocalX(float r);
		void RotateLocalY(float r);
		void RotateLocalZ(float r);

		void RotateGlobalX(float r);
		void RotateGlobalY(float r);
		void RotateGlobalZ(float r);

		void DirtyTransform(void);

		bool wasModified(void);

		void LoadState(TiXmlElement* xmlElement);

		GameObject* Find(const char* name);
		GameObject* FindInHierarchy(const char* name);
		GameObject* FindInHierarchy(RegistrationId nameHash);

		//static GameObject* Find(Transform* transform, const char* name);
		GameObject* Find(RegistrationId nameHash);
		//GameObject* Find(RegistrationId* rgNameHashes, unsigned int count);

		template<typename Func>
		static void ForEachTransformInHierarchy(Transform* transform, Func func)
		{
			func( transform );

			for( transform = transform->firstChild; transform != 0; transform = transform->nextSibling )
				ForEachTransformInHierarchy(transform, func);
		}

		Transform* clone(void)const
		{
			return new Transform(*this);
		}

	private:
		Transform* parent;
		
		Transform* firstChild;
		Transform* nextSibling;

		Float4x4 worldMatrix;
		Float4x4 localMatrix;

		void RemoveChild(Transform* child);
		const Float4x4& _GetWorldMatrix(void);
		const Float4x4& _GetWorldMatrixSolution(void);
		void OnDestroy(void);

		bool dirtied;
		bool modified;
	};
}