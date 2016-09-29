#include "stdafx.h"

#include "AIController.h"
#include "AIEvaluator.h"
#include "Game.h"
#include "System.h"
#include "Tetris.h"

GameManager::~GameManager()
{
	for (int i = 0; i < m_games.size(); i++)
	{		
		delete m_games[i];
		m_games.erase(m_games.begin() + i);
		i--;
	}	
}

void GameManager::Update(float dt)
{
	for (auto T : m_games)
	{
		T->Update(dt);
	}
}

void GameManager::Draw(sf::RenderWindow * window, float dt)
{
	window->clear(sf::Color(40, 40, 40));

	for (auto T : m_games)
	{
		T->Draw(window, dt);
	}
}

bool GameManager::IsRunning()
{
	if (m_games.size() > 0)
	{
		return m_games.at(0)->m_game->IsRunning();
	}
	return false;
}

void GameInfo::Update(float dt)
{
	m_game->Update(dt);

	for (auto S : m_systems)
	{
		S->Update(dt);
	}

}

void GameInfo::Draw(sf::RenderWindow * window, float dt)
{
	window->setView(m_view);
	m_game->Draw(window);
}

void AIGameInfo::Update(float dt)
{
	GameInfo::Update(dt);
	if (m_evaluator && 
		m_evaluator->CanFindMove() && 
		m_controller && 
		m_controller->NeedsNewMove())
	{
		m_evaluator->FindBestMove();
		m_controller->SetCurrentMove(m_evaluator->GetBestMove());
/*		printf("-----------\n");

		for (int i = 0; i < m_evaluator->m_debugHeuristics.size(); i++)
		{
			printf("Score [%s] [%.2f]\n", m_evaluator->m_debugHeuristics[i].m_description.c_str(), m_evaluator->m_debugHeuristics[i].m_lastScore);
		}*/
	}

}

void AIGameInfo::Draw(sf::RenderWindow * window, float dt)
{
	GameInfo::Draw(window, dt);
	// Debug
	sf::Text text;
	text.setFont(m_game->m_debugFont);
	text.setCharacterSize(16);
	text.setColor(sf::Color::White);

	if (m_evaluator)
	{
		for (int i = 0; i < m_evaluator->m_debugHeuristics.size(); i++)
		{
			std::string textStr = m_evaluator->m_debugHeuristics[i].m_description;
			textStr += " " + std::to_string(m_evaluator->m_debugHeuristics[i].m_lastScore);
			text.setPosition(sf::Vector2f(50, 250 + i * 20));
			text.setString(textStr.c_str());
			window->draw(text);
		}
	}
}
