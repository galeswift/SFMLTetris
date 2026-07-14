#include "stdafx.h"
#include "AIEvaluatorComponent.h"
#include "AIHeuristics.h"
#include "ComponentPool.h"

DEFINE_COMPONENT(AIEvaluatorComponent)

AIEvaluatorComponent::~AIEvaluatorComponent()
{
	ClearHeuristics();
}

void AIEvaluatorComponent::Reset()
{
	ClearHeuristics();
	m_debugHeuristics.clear();
	m_initialized = false;
	m_aiHeuristicRange = sf::Vector2f(1.0f, 1.0f);
	m_timeSinceLastUpdate = 0.0f + rand() / (float)RAND_MAX;
	for (int i = 0; i < NUM_LOOKAHEAD; i++)
	{
		m_bestMoves[i] = DesiredMoveSet();
	}
}

void AIEvaluatorComponent::ClearHeuristics()
{
	for (s32 i = 0; i < m_heuristics.size(); i++)
	{
		delete m_heuristics[i];
	}
	m_heuristics.clear();
}
