#pragma once

#include "AICommon.h"
#include "System.h"
#include <string>

class Tetris;

class AIEvaluatorSystem : public System
{
public:
	AIEvaluatorSystem();	
	void Update(float dt);		
};