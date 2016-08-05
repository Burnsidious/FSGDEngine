#pragma once

#include "../EDGameCore/Behavior.h"
#include "../EDGameCore/Message.h"


class TerrainController : public EDGameCore::Behavior<TerrainController>
{
protected:
	void Awake(void);
	void Update(void);

public:
	TerrainController();
	~TerrainController();
};

