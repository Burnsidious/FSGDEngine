#pragma once

namespace EDGameCore
{
	enum ColliderType{ INVALID = -1, SPHERE, CAPSULE, BOX, CHARACTER, WHEEL, MESH };
	enum ColliderCombination{ SC = 0, RC, RCS, KRC, STC, RTC, KRTC, CC_COUNT };
	//enum ColliderInteraction{ NONE = 0, RC_TO_RC, RC_TO_RCS, RC_TO_SC, RC_TO_KRC, RC_TO_STC, KRC_TO_SC, RC_TO_RTC, RTC_TO_RTC };
}