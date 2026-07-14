#include "stdafx.h"
#include "AIHeuristics.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorSystem.h"
#include "ClientGame.h"
#include "Tetris.h"

#define RAND_HEURISTIC_RANGE() (rand()/float(RAND_MAX)* flHeuristicWidth + comp->m_aiHeuristicRange.x)
void AIEvaluatorSystem::Update(GameManager& manager, float dt)
{
	for (ComponentIterator itr = manager.GetComponents(COMPONENT_AI_EVALUATOR); itr.Get() != nullptr; itr++)
	{
		AIEvaluatorComponent* comp = itr.Get<AIEvaluatorComponent>();
		if (!comp->m_initialized)
		{
			float flHeuristicWidth = comp->m_aiHeuristicRange.y - comp->m_aiHeuristicRange.x;
			comp->m_initialized = true;
			comp->m_heuristics.push_back(new AIHeuristic_AggregateHeight(-2.00 * RAND_HEURISTIC_RANGE()));
			//comp->m_heuristics.push_back(new AIHeuristic_CompletedLines(2));
			//comp->m_heuristics.push_back(new AIHeuristic_HighestCol(-2.75));
			//comp->m_heuristics.push_back(new AIHeuristic_DeepestHole(2.0 * RAND_HEURISTIC_RANGE()));
			comp->m_heuristics.push_back(new AIHeuristic_GameLoss(-1 * RAND_HEURISTIC_RANGE()));
			comp->m_heuristics.push_back(new AIHeuristic_Holes(-5.5f));
			comp->m_heuristics.push_back(new AIHeuristic_Blockade(-.4f * RAND_HEURISTIC_RANGE()));
			comp->m_heuristics.push_back(new AIHeuristic_Bumpiness(-1.55f * RAND_HEURISTIC_RANGE()));
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Agg Height" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Game Loss" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Holes" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Blockade" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Bumpiness" });
		}

		comp->m_timeSinceLastUpdate += dt;
	}
}
