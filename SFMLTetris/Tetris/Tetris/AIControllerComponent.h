#pragma once

#include "AICommon.h"
#include "Component.h"

#define AI_CONTROLLER_UPDATE_FREQUENCY (.00f)

class Tetris;
class AIControllerComponent : public Component
{
public:
	AIControllerComponent(Tetris* owner);
	virtual ~AIControllerComponent();

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

	Tetris* m_tetrisBoard;
	DesiredMoveSet m_currentMove;
	float m_timeUntilUpdate;
	float m_updateFrequency;
};

