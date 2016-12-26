#pragma once
#include "System.h"
class AISpawnSystem :
	public System
{
public:
	AISpawnSystem();
	~AISpawnSystem();
	void Update(float dt) override;
};

