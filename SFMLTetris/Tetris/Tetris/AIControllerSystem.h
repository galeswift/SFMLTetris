#pragma once

#include "AICommon.h"
#include "System.h"

class Tetris;

// Controls the movement of a piece based on a desired position
class AIControllerSystem : public System
{

public:
	AIControllerSystem();
	void Update(float dt);	
private:
};
