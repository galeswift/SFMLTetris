#include "stdafx.h"
#include "AIHeuristics.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorSystem.h"
#include "ClientGame.h"
#include "Tetris.h"

AIEvaluatorSystem::AIEvaluatorSystem()	
{	
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		AIEvaluatorComponent* comp = current->GetComponent<AIEvaluatorComponent>();
		if (comp)
		{
			comp->m_heuristics.push_back(new AIHeuristic_AggregateHeight(-2.00));
			//comp->m_heuristics.push_back(new AIHeuristic_CompletedLines(2));
			//comp->m_heuristics.push_back(new AIHeuristic_HighestCol(-2.75));
			comp->m_heuristics.push_back(new AIHeuristic_GameLoss(-1));
			comp->m_heuristics.push_back(new AIHeuristic_Holes(-5.5f));
			comp->m_heuristics.push_back(new AIHeuristic_Blockade(-.4f));
			comp->m_heuristics.push_back(new AIHeuristic_Bumpiness(-1.55f));			
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Agg Height" });
			//comp->m_debugHeuristics.push_back(AIDebug{ 0.0f, "Completed Lines" });
			//comp->m_debugHeuristics.push_back(AIDebug{ 0.0f, "Highest Column" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Game Loss" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Holes" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Blockade" });
			comp->m_debugHeuristics.push_back(AIDebugHeuristic{ 0.0f, "Bumpiness" });
		}
	}	
}

void AIEvaluatorSystem::Update(float dt)
{
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		AIEvaluatorComponent* comp = current->GetComponent<AIEvaluatorComponent>();
		if (comp)
		{
			comp->m_timeSinceLastUpdate += dt;
		}
	}	
}

