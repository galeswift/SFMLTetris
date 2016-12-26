#include "stdafx.h"
#include "AIControllerComponent.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorUtil.h"
#include "AIHeuristics.h"
#include "AIMoveSystem.h"
#include "Tetris.h"

AIMoveSystem::AIMoveSystem()
{
}


AIMoveSystem::~AIMoveSystem()
{
}

void AIMoveSystem::Update(float dt)
{
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		AIControllerComponent* controlComp = current->GetComponent<AIControllerComponent>();
		AIEvaluatorComponent* evalComp = current->GetComponent<AIEvaluatorComponent>();
		if (evalComp &&
			evalComp->CanFindMove() && 
			controlComp &&
			controlComp->NeedsNewMove())
		{
			AIEvaluatorUtil::FindBestMove(evalComp);
			controlComp->SetCurrentMove(evalComp->GetBestMove());
		}
	}		
}

void AIMoveSystem::Draw(sf::RenderWindow * window, float dt)
{
	// Debug
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		AIEvaluatorComponent* evalComp = current->GetComponent<AIEvaluatorComponent>();
		if (evalComp)
		{ 
			sf::Text text;
			text.setFont(evalComp->m_owner->m_debugFont);
			text.setCharacterSize(16);
			text.setColor(sf::Color::White);

			for (int i = 0; i < evalComp->m_debugHeuristics.size(); i++)
			{
				std::string textStr = evalComp->m_debugHeuristics[i].m_description;
				textStr += " " + std::to_string(evalComp->m_debugHeuristics[i].m_lastScore);
				text.setPosition(sf::Vector2f(50, (float)250 + i * 20));
				text.setString(textStr.c_str());
				window->draw(text);
			}
		}
	}	
}
