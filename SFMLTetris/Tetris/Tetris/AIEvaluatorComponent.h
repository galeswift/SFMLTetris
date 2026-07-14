#pragma once
#include "AICommon.h"
#include "AIHeuristics.h"
#include "Component.h"

#define NUM_LOOKAHEAD (2)
#define AI_UPDATE_RATE_SECONDS (0.0f)
class AIEvaluatorComponent : public Component
{
	DECLARE_COMPONENT(COMPONENT_AI_EVALUATOR)

	virtual ~AIEvaluatorComponent();
	virtual void Reset();
	void ClearHeuristics();

	bool CanFindMove() { return m_timeSinceLastUpdate > AI_UPDATE_RATE_SECONDS; }
	DesiredMoveSet GetBestMove() { return m_bestMoves[0]; }
	bool m_initialized;
	float m_timeSinceLastUpdate;
	sf::Vector2f m_aiHeuristicRange;
	std::vector<AIHeuristic*> m_heuristics;
	std::vector<AIDebugHeuristic> m_debugHeuristics;
	DesiredMoveSet m_bestMoves[NUM_LOOKAHEAD];
};
