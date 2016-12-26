#pragma once
#include <vector>
#include "Component.h"

class AIControllerSystem;
class AIEvaluatorSystem;
class AISpawnComponent;
class Component;
class PlayerComponent;
class System;
class Tetris;

struct GameInfo
{
	GameInfo()
		: m_game(nullptr)
		, m_handle(0)
		, m_pos(0,0)
	{

	}

	~GameInfo();

	virtual void Update(float dt);	
	virtual void Draw(sf::RenderWindow* window, float dt);
	u32 m_handle;
	Tetris* m_game;	
	sf::View m_view;
	sf::Vector2f m_pos;
};

class GameManager
{
public:
	~GameManager();
	template <typename T>
	T* AddGame(Tetris* game, const sf::FloatRect& rect, GameHandle handle);
	void Update(float dt);
	void Draw(sf::RenderWindow* window, float dt);
	void RemoveGame(GameHandle handle);
	bool IsRunning();
	u32 ReserveHandle() { return m_handleIdx++; }
	AISpawnComponent* GetSpawnComponent();

	std::vector<Component*> m_components;
	std::vector<System*> m_systems;
	std::vector<GameInfo*> m_games;

	u32 m_handleIdx;
};

template <typename T>
T* GameManager::AddGame(Tetris* game, const sf::FloatRect& rect, GameHandle handle)
{
	T* newInfo = new T();
	sf::FloatRect viewportRect = rect;
	newInfo->m_game = game;
	newInfo->m_view.reset(sf::FloatRect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT));
	viewportRect = sf::FloatRect(rect.left / WINDOW_WIDTH, rect.top / WINDOW_HEIGHT, rect.width, rect.height);
	newInfo->m_view.setViewport(viewportRect);
	newInfo->m_handle = handle;
	newInfo->m_pos = sf::Vector2f(rect.left, rect.top);
	m_games.push_back(newInfo);
	
	return newInfo;
}

extern GameManager g_clientGame;