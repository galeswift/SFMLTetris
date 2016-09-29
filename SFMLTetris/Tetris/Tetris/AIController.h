#pragma once

#include "AICommon.h"
#include "System.h"

class Tetris;

#define AI_CONTROLLER_UPDATE_FREQUENCY (.00f)

// Controls the movement of a piece based on a desired position
class AIController : public System
{

public:
	AIController(Tetris* tetrisBoard);
	void Update(float dt);
	void SetUpdateFrequency(float time) { m_updateFrequency = time; }
	void SetCurrentMove(DesiredMoveSet& move) 
	{ 
		if (move.id != m_currentMove.id)
		{
			m_currentMove = move;
		}
	}	
	bool NeedsNewMove()
	{
		return m_currentMove.used;
	}

private:
	Tetris* m_tetrisBoard;	
	DesiredMoveSet m_currentMove;
	float m_timeUntilUpdate;
	float m_updateFrequency;
};
