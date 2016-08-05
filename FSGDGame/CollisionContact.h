#pragma once
#include "../EDMath/Float3.h"

struct CollisionContact
{
	EDMath::Float3 m_ContactPoint;
	EDMath::Float3 m_ContactNormal;
	float m_PenetrationDepth;
};
