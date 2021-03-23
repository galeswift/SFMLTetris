#include "stdafx.h"

#include "AIControllerSystem.h"
#include "AIEvaluatorSystem.h"
#include "AISpawnComponent.h"
#include "ClientGame.h"
#include "System.h"
#include "Tetris.h"

GameManager g_clientGame = GameManager();

GameManager::~GameManager()
{
	// SFML Crashes in cleaning up the font here..
	//for (int i = 0; i < m_games.size(); i++)
	//{		
	//	delete m_games[i];
	//	m_games.erase(m_games.begin() + i);
	//	i--;
	//}	

	//for (int i = 0; i < m_systems.size(); i++)
	//{
	//	delete m_systems[i];
	//	m_systems.erase(m_systems.begin() + i);
	//	i--;
	//}
}

void GameManager::Update(float dt)
{
	for (auto S : m_systems)
	{
		S->Update(dt);
	}

	for (auto T : m_games)
	{
		T->Update(dt);
	}	
}

void GameManager::Draw(sf::RenderWindow * window, float dt)
{
	window->clear(WINDOW_CLEAR_COLOR);

	for (auto S : m_systems)
	{
		S->Draw(window, dt);
	}

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

AISpawnComponent* GameManager::GetSpawnComponent()
{
	AISpawnComponent* result = NULL;
	for (s32 i = 0; i < m_components.size(); i++)
	{
		result = dynamic_cast<AISpawnComponent*>(m_components[i]);
		if (result != NULL)
		{
			break;
		}
	}
	return result;
}

void GameManager::RemoveGame(GameHandle handle)
{
	for (s32 i = 0; i < m_games.size(); i++)
	{
		if (m_games[i]->m_handle == handle)
		{
			delete m_games[i];
			m_games.erase(m_games.begin() + i);
			break;
		}
	}
}

GameInfo::~GameInfo()
{
	delete m_game;
	m_game = NULL;
}

void GameInfo::Update(float dt)
{
	m_game->Update(dt);
}

void GameInfo::Draw(sf::RenderWindow * window, float dt)
{
	window->setView(m_view);
	m_game->Draw(window);
}