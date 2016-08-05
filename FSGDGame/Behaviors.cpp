#include "Behaviors.h"
#include "Controller.h"
#include "Renderable.h"
#include "Collider.h"
#include "Physics.h"
#include "GroundClamp.h"
#include "CharacterController.h"
#include "KeyboardBehavior.h"
#include "JoystickBehavior.h"
#include "TranslatorBehavior.h"
#include "MouseLookBehavior.h"
#include "HardAttachBehavior.h"
#include "LookAtBehavior.h"
#include "TurnToBehavior.h"
#include "PidFollower.h"
#include "CameraBehavior.h"
#include "DirectionalLightController.h"
#include "LightController.h"
#include "SkyBoxBehavior.h"
#include "SpinBehavior.h"
#include "PointLightBehavior.h"
#include "SpotLightBehavior.h"
#include "BaseRenderable.h"
#include "TurretBehavior.h"
#include "WheelBehavior.h"
#include "ProjectileBehavior.h"
#include "ShootingBehavior.h"
#include "AnimationBehavior.h"

using namespace EDGOCA;

typedef Behaviors::GroundClamp ZGroundClamp;

namespace Behaviors
{
	DeclareBehavior( Controller, BHVR_CONTROLLER );
	DeclareBehavior( Renderable, BHVR_RENDERABLE );
	DeclareBehavior( Collider, BHVR_COLLIDER );
	DeclareBehavior( Physics, BHVR_PHYSICS );
	DeclareBehavior( GroundClamp, BHVR_GROUNDCLAMP );
	DeclareBehavior( CharacterController, BHVR_CHARACTER_CONTROLLER );

	DeclareBehavior( TranslatorBehavior, BHVR_TRANSLATOR_CONTROLLER );

	DeclareBehavior( KeyboardBehavior, BHVR_KEYBOARD );
	DeclareBehavior( JoystickBehavior, BHVR_JOYSTICK );
	
	DeclareBehavior( MouseLookBehavior, BHVR_MOUSE_LOOK );
	DeclareBehavior( HardAttachBehavior, BHVR_HARD_ATTACH );
	DeclareBehavior( CameraBehavior, BHVR_CAMERA );

	DeclareBehavior( DirectionalLightController, BHVR_DIRECTIONAL_LIGHT );
	DeclareBehavior( LightController, BHVR_LIGHT );

	DeclareBehavior( SkyBoxBehavior, BHVR_SKY_BOX );
	DeclareBehavior( SpinBehavior, BHVR_SPIN );

	DeclareBehavior( LookAtBehavior, BHVR_LOOK_AT );
	DeclareBehavior( TurnToBehavior, BHVR_TURN_TO );
	DeclareBehavior( PIDFollower, BHVR_PID_FOLLOWER );

	DeclareBehavior( PointLightBehavior, BHVR_POINT_LIGHT );
	DeclareBehavior( SpotLightBehavior, BHVR_SPOT_LIGHT );

	DeclareBehavior( BaseRenderable, BVHR_BASE_RENDERABLE );

	DeclareBehavior( TurretBehavior, BHVR_TURRET_CONTROLLER );
	DeclareBehavior( WheelBehavior, BHVR_WHEEL );

	DeclareBehavior( ProjectileBehavior, BHVR_PROJECTILE );
	DeclareBehavior( ShootingBehavior, BHVR_SHOOTING );
	
	DeclareBehavior( AnimationBehavior, BHVR_ANIMATION );
}
