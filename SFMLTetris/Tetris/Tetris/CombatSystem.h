#pragma once
#include "System.h"

class CombatSystem : public System
{
public:
	void Update(GameManager& manager, float dt) override;
};
