#pragma once
#include "System.h"

class AISpawnSystem : public System
{
public:
	void Update(GameManager& manager, float dt) override;
};
