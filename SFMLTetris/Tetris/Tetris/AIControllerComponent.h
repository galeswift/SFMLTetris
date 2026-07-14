#pragma once

#include "AICommon.h"
#include "Component.h"

#define AI_CONTROLLER_UPDATE_FREQUENCY (.00f)

class AIControllerComponent : public Component
{
	DECLARE_COMPONENT(COMPONENT_AI_CONTROLLER)

	AIControllerComponent();
	virtual void Reset();

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

	DesiredMoveSet m_currentMove;
	float m_timeUntilUpdate;
	float m_updateFrequency;
};
