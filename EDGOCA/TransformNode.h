#pragma once


#include "../EDMath/Float4x4.h"

namespace EDGOCA
{
	class TransformNode
	{
		friend class GameObject;
	public:
		enum NodeType{ SCENE_NODE = 0, TRANSFORM_NODE, GAME_OBJECT };

		enum TransformFlag{ DIRTY = 1, INHERIT_XFORM_POS = 2, INHERIT_XFORM_ROT = 4, INHERIT_XFORM = 6 };

		TransformNode(void) : parentPtr(0), transformFlags( INHERIT_XFORM )
		{
			localTransform.makeIdentity();
			worldTransform.makeIdentity();
		}

		inline const EDMath::Float4x4& GetLocalTransform(void) const { return localTransform; }

		const EDMath::Float4x4& GetWorldTransform(void);

		inline unsigned int GetTransformFlags(void){ return transformFlags; }

		void SetTransformInheritence(unsigned int flags)
		{
			transformFlags = flags;
			DirtyNode();
		}

		virtual void TranslateLocal(const EDMath::Float3& delta);
		virtual void TranslateGlobal(const EDMath::Float3& delta);
		
		virtual void RotateLocalX(float radians);
		virtual void RotateLocalY(float radians);
		virtual void RotateLocalZ(float radians);

		virtual void RotateGlobalX(float radians);
		virtual void RotateGlobalY(float radians);
		virtual void RotateGlobalZ(float radians);

		virtual void SetLocalTransform(const EDMath::Float4x4& transform);
		virtual void SetLocalTransform(EDMath::Float4x4 &&transform);
		virtual void SetLocalPosition(const EDMath::Float3& position);

		TransformNode* GetRoot(void);

		TransformNode* GetParent(void){ return parentPtr; }

		NodeType GetNodeType(void){ return TRANSFORM_NODE; }

		virtual ~TransformNode();

		inline const std::set<TransformNode*>& GetChildren(void) const { return children; }

	private:
		EDMath::Float4x4 localTransform;
		EDMath::Float4x4 worldTransform;
		unsigned int transformFlags;
		TransformNode* parentPtr;
		std::set<TransformNode*> children;

		const EDMath::Float4x4& GetWorldTransformSolution(void);
		void DirtyNodeSolution(void);

	protected:
		virtual void DirtyNode(void);

		void AddChild(TransformNode* childNodePtr);
		void RemoveChild(TransformNode* childNodePtr);
	};
}