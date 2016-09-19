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

void GameManager::Update(sf::RenderWindow* window, float dt)
{
	window->clear(sf::Color(40, 40, 40));

	for (auto T : m_games)
	{
		T->Update(window, dt);
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

void GameInfo::Update(sf::RenderWindow * window, float dt)
{
	m_game->Update(dt);

	for (auto S : m_systems)
	{
		S->Update(dt);
	}

	window->setView(m_view);
	m_game->Draw(window);
}

void AIGameInfo::Update(sf::RenderWindow * window, float dt)
{
	GameInfo::Update(window, dt);
	if (m_evaluator && 
		m_evaluator->CanFindMove() && 
		m_controller && 
		m_controller->NeedsNewMove())
	{
		m_evaluator->FindBestMove();
		m_controller->SetCurrentMove(m_evaluator->GetBestMove());
	}

	// Debug
	sf::Text text;	
	text.setFont(m_game->m_debugFont);
	text.setCharacterSize(18);
	text.setColor(sf::Color::White);

	for (int i = 0; i < m_evaluator->m_debugHeuristics.size(); i++)
	{		
		std::string textStr = m_evaluator->m_debugHeuristics[i].m_description;
		textStr += " " + std::to_string(m_evaluator->m_debugHeuristics[i].m_lastScore);
		text.setPosition(sf::Vector2f(350, 150 + i * 30));
		text.setString(textStr.c_str());
		window->draw(text);
	}
}
