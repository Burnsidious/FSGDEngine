#include "../EDGameCore/Transform.h"

namespace EDGameCore
{
	void Transform::TranslateGlobalSolution(const Float3& translation)
	{
		localMatrix.WAxis += translation;
	}
	
	void Transform::TranslateLocalSolution(const Float3& translation)
	{
		localMatrix.WAxis += localMatrix.XAxis * translation.x;
		localMatrix.WAxis += localMatrix.YAxis * translation.y;
		localMatrix.WAxis += localMatrix.ZAxis * translation.z;
	}
}

/*
#include "precompiled.h"

#include "../FSGDGame/TranslatorBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDUtilities/PrintConsole.h"

using namespace EDMath;
using namespace EDGOCA;
using namespace EDUtilities;

namespace Behaviors
{
	void TranslatorBehavior::OnMoveForwardSolution( IBehavior* invokingBehavior, IMessage* message )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"TranslatorBehavior::OnMoveForwardSolution");
			added = true;
		}

		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// translate on axis based on speed (speed scalar * game time)
		controller->gameObject->TranslateLocal( Float3( 0.0f, 0.0f, 1.0f ) * controller->ComputeSpeed()  );
	}

	void TranslatorBehavior::OnMoveBackwardSolution( IBehavior* invokingBehavior, IMessage* message )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"TranslatorBehavior::OnMoveBackwardSolution");
			added = true;
		}

		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// translate on axis based on speed (speed scalar * game time)
		controller->gameObject->TranslateLocal( Float3( 0.0f, 0.0f, -1.0f ) * controller->ComputeSpeed() );
	}

	void TranslatorBehavior::OnMoveLeftSolution( IBehavior* invokingBehavior, IMessage* message )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"TranslatorBehavior::OnMoveLeftSolution");
			added = true;
		}

		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// translate on axis based on speed (speed scalar * game time)
		controller->gameObject->TranslateLocal( Float3( -1.0f, 0.0f, 0.00f ) * controller->ComputeSpeed() );
	}

	void TranslatorBehavior::OnMoveRightSolution( IBehavior* invokingBehavior, IMessage* message )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"TranslatorBehavior::OnMoveRightSolution");
			added = true;
		}

		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// translate on axis based on speed (speed scalar * game time)
		controller->gameObject->TranslateLocal( Float3( 1.0f, 0.0f, 0.0f ) * controller->ComputeSpeed() );
	}

	void TranslatorBehavior::OnMoveUpSolution( IBehavior* invokingBehavior, IMessage* message )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"TranslatorBehavior::OnMoveUpSolution");
			added = true;
		}

		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// translate on axis based on speed (speed scalar * game time)
		controller->gameObject->TranslateLocal( Float3( 0.0f, 1.0f, 0.0f ) * controller->ComputeSpeed() );
	}

	void TranslatorBehavior::OnMoveDownSolution( IBehavior* invokingBehavior, IMessage* message )
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Matrix", Float3(1.0f, 0.0f, 0.0f), L"TranslatorBehavior::OnMoveDownSolution");
			added = true;
		}

		TranslatorBehavior* controller = (TranslatorBehavior*)invokingBehavior;

		// translate on axis based on speed (speed scalar * game time)
		controller->gameObject->TranslateLocal( Float3( 0.0f, -1.0f, 0.0f ) * controller->ComputeSpeed() );
	}
}
*/