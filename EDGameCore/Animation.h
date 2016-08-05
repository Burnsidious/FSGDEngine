#pragma once

#include "Component.h"
#include "GameObject.h"
#include "../RTAnim/Interpolator.h"
#include "../EDUtilities/ContentManager.h"
#include <unordered_map>

namespace EDGameCore
{
	class Animation : public ComponentT<Animation>
	{
		friend class GameObject;
		friend class Game;

	public:
		Animation(void) : animatePhysics(false) {}
		~Animation();

		ComponentType GetComponentType(void)const{ return ANIMATION; }
		
		void AddClip(const char* filePath);

		void Play(const char* filePath);
		void Play(RegistrationId clipPathId);

		const std::vector<EDMath::Float4x4>& GetFlatMats(void);

		Animation* clone(void)const
		{
			return new Animation(*this);
		}

		bool AnimatesPhysics(void){ return animatePhysics; }
		void SetAnimatesPhysics(bool b){ animatePhysics = b; }

	private:
		void Update(void);

		bool animatePhysics;

		RTAnim::Interpolator interpolator;

		std::vector<EDMath::Float4x4> flatMats;

		std::unordered_map< RegistrationId, EDUtilities::ContentHandle<RTAnim::Clip> > clipMap;

		void LoadState(TiXmlElement* xmlElement);
	};
}
