#pragma once

#include "System.h"

// Controls the movement of a piece based on a desired position
class AIControllerSystem : public System
{
public:
	void Update(GameManager& manager, float dt) override;
};
