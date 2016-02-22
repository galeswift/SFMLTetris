#include "stdafx.h"

#include "Game.h"
#include "Tetris.h"

GameManager::~GameManager()
{
	m_games.clear();
}

void GameManager::AddGame(Tetris* game, const sf::FloatRect& rect)
{
	GameInfo newInfo;
	newInfo.game = game;	
	newInfo.view.reset(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	newInfo.view.setViewport(rect);

	m_games.push_back(newInfo);
}

void GameManager::Update(sf::RenderWindow* window, float dt)
{
	window->clear(sf::Color(40, 40, 40));
	for (auto T : m_games)
	{
		T.game->Update(dt);
		window->setView(T.view);		
		T.game->Draw(window);

	}
}

bool GameManager::IsRunning()
{
	if (m_games.size() > 0)
	{
		return m_games[0].game->IsRunning();
	}
	return false;
}
