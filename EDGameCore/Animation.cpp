#include "Animation.h"
#include "Game.h"
#include "Transform.h"
#include <functional>
#include <string>

namespace EDGameCore
{
	Animation::~Animation()
	{
		GameObject* gameObject = GetGameObject();
		if( gameObject && gameObject->GetAnimation() == this )
			gameObject->SetAnimation((Animation*)nullptr);
	}

	void Animation::AddClip(const char* filePath)
	{
		RegistrationId hashId = (unsigned int)std::hash< std::string >()(filePath);

		ContentHandle< RTAnim::Clip > clipHandle = ContentManager::Load< RTAnim::Clip >( filePath );

		RTAnim::Clip* clipPtr = clipHandle.GetContent();

		if( clipPtr != 0 )
		{
			clipMap.insert( std::make_pair( hashId, clipHandle ) );
	
			if( clipMap.size() == 1 )
			{
				auto& bones = clipPtr->GetBones();
				unsigned int boneCount = clipPtr->GetNumBones();
				flatMats.resize( boneCount );

				std::vector< EDGameCore::GameObject* > gameObjSet;

				for(unsigned int i = 0; i < boneCount; ++i)
				{
					EDMath::Float4x4 localMat = bones[i].GetKeyFrames()[0].GetLocalTransform();
					EDGameCore::GameObject* gameObj = EDGameCore::Game::create< EDGameCore::GameObject >();
					gameObj->SetName( bones[i].GetName() );
					gameObjSet.push_back( gameObj );

					if( bones[i].GetParentIndex() != -1 )
					{
						EDMath::Multiply( flatMats[i], localMat, flatMats[ bones[i].GetParentIndex() ] );
						gameObj->GetTransform()->SetLocalMatrix( flatMats[i] );
						
						gameObj->GetTransform()->SetParent( gameObjSet[ bones[i].GetParentIndex() ]->GetTransform() );
					}
					else
					{
						//flatMats[i] = localMat;
						EDMath::Multiply( flatMats[i], localMat, GetTransform()->GetWorldMatrix() );
						gameObj->GetTransform()->SetLocalMatrix( flatMats[i] );
						gameObj->GetTransform()->SetParent( GetTransform() );
					}
				}

				Play( hashId );
			}
		}
	}

	void Animation::Play(const char* filePath)
	{
		RegistrationId hashId = (unsigned int)std::hash< std::string >()(filePath);
		
		Play( hashId );
	}

	void Animation::Play(RegistrationId clipPathId)
	{
		auto iter = clipMap.find( clipPathId );

		if( iter != clipMap.end() )
			interpolator.SetClip( iter->second );
	}

	void Animation::Update(void)
	{
		interpolator.AddTime( EDGameCore::Game::GetDeltaTime() );

		const RTAnim::Clip* clipPtr = ((const RTAnim::Interpolator&)interpolator).GetClip();

		if( clipPtr != 0 )
		{
			interpolator.Process();

			auto& interpolatedBones = interpolator.GetInterpolatedBones();
			auto& bindTransforms = clipPtr->GetBindTransforms();
			unsigned int boneCount = clipPtr->GetNumBones();

			for(unsigned int i = 0; i < boneCount; ++i)
			{
				const RTAnim::AnimatedBone* animatedBone = interpolatedBones[i].GetAnimatedBone();

				// Set local transforms of gameObjects...
				EDGameCore::GameObject* gameObj = GetTransform()->FindInHierarchy( animatedBone->GetNameHash() );
				if( gameObj != 0 )
					gameObj->GetTransform()->SetLocalMatrix( interpolatedBones[i].GetLocalTransform() );
			}
		}
	}

	const std::vector<EDMath::Float4x4>& Animation::GetFlatMats(void)
	{
		const RTAnim::Clip* clipPtr = ((const RTAnim::Interpolator&)interpolator).GetClip();

		if( clipPtr != 0 )
		{
			auto& interpolatedBones = interpolator.GetInterpolatedBones();
			auto& bindTransforms = clipPtr->GetBindTransforms();
			unsigned int boneCount = clipPtr->GetNumBones();

			if( flatMats.size() != boneCount )
				flatMats.resize( boneCount );

			for(unsigned int i = 0; i < boneCount; ++i)
			{
				const RTAnim::AnimatedBone* animatedBone = interpolatedBones[i].GetAnimatedBone();

				// Get world transforms of gameObjects...
				EDGameCore::GameObject* gameObj = GetTransform()->FindInHierarchy( animatedBone->GetNameHash() );
				
				if( gameObj != 0 )
					EDMath::Multiply( flatMats[i], bindTransforms[i], gameObj->GetTransform()->GetWorldMatrix() );	
			}
		}

		return flatMats;
	}

	void Animation::LoadState(TiXmlElement* xmlElement)
	{
		TiXmlElement* clipEle = xmlElement->FirstChildElement("Clip");

		for(; clipEle != 0; clipEle = clipEle->NextSiblingElement("Clip") )
		{
			if( clipEle->GetText() != 0 &&
				strlen( clipEle->GetText() ) != 0 )
				AddClip( clipEle->GetText() );
		}
	}
}