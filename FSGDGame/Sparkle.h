#pragma once

#include "../EDGameCore/Behavior.h"

class Sparkle : public EDGameCore::Behavior<Sparkle>
{
	static const unsigned int SPARKLE_COUNT = 64;

	Float3 points[SPARKLE_COUNT][2];

	void Update(void);

public:
	Sparkle(void);
};