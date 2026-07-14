#include "stdafx.h"

#include "ClientGame.h"
#include "PlayerComponent.h"
#include "System.h"
#include "Tetris.h"

ComponentPoolFactory ComponentRegistry::s_factories[COMPONENT_TYPE_COUNT];

void ComponentRegistry::Register(ComponentType type, ComponentPoolFactory factory)
{
	s_factories[type] = factory;
}

ComponentPoolBase* ComponentRegistry::CreatePool(ComponentType type)
{
	return s_factories[type] ? s_factories[type]() : NULL;
}

GameManager::GameManager()
	: m_handleIdx(0)
{
	for (int i = 0; i < COMPONENT_TYPE_COUNT; i++)
	{
		m_pools[i] = ComponentRegistry::CreatePool((ComponentType)i);
	}
}

GameManager::~GameManager()
{
	for (s32 i = 0; i < m_games.size(); i++)
	{
		FreeComponents(m_games[i]->m_game);
		delete m_games[i];
	}
	m_games.clear();

	for (int i = 0; i < COMPONENT_TYPE_COUNT; i++)
	{
		delete m_pools[i];
		m_pools[i] = NULL;
	}

	for (s32 i = 0; i < m_systems.size(); i++)
	{
		delete m_systems[i];
	}
	m_systems.clear();
}

void GameManager::Update(float dt)
{
	for (auto S : m_systems)
	{
		S->Update(*this, dt);
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
		S->Draw(*this, window, dt);
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

Component* GameManager::AddComponent(ComponentType type, Tetris* game)
{
	Component* component = m_pools[type]->AllocBase(game);
	game->m_componentSlots[type] = component;
	return component;
}

ComponentIterator GameManager::GetComponents(ComponentType type)
{
	return ComponentIterator(m_pools[type]);
}

void GameManager::FreeComponents(Tetris* game)
{
	for (int i = 0; i < COMPONENT_TYPE_COUNT; i++)
	{
		if (game->m_componentSlots[i])
		{
			m_pools[i]->Free(game->m_componentSlots[i]);
			game->m_componentSlots[i] = NULL;
		}
	}
}

void GameManager::RemoveGame(GameHandle handle)
{
	for (s32 i = 0; i < m_games.size(); i++)
	{
		if (m_games[i]->m_handle == handle)
		{
			FreeComponents(m_games[i]->m_game);
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
