#pragma once
#include <vector>
#include "Component.h"
#include "Constants.h"

class Tetris;
class AIEvaluatorSystem;
class AIControllerSystem;
class PlayerComponent;
class System;

struct GameInfo
{
	GameInfo()
		: m_game(nullptr)
	{

	}

	~GameInfo();

	virtual void Update(float dt);	
	virtual void Draw(sf::RenderWindow* window, float dt);
	Tetris* m_game;	
	sf::View m_view;
};

class GameManager
{
public:
	~GameManager();
	template <typename T>
	T* AddGame(Tetris* game, const sf::FloatRect& rect);	
	void Update(float dt);
	void Draw(sf::RenderWindow* window, float dt);
	bool IsRunning();

	std::vector<System*> m_systems;
	std::vector<GameInfo*> m_games;	
};

template <typename T>
T* GameManager::AddGame(Tetris* game, const sf::FloatRect& rect)
{
	T* newInfo = new T();
	newInfo->m_game = game;
	newInfo->m_view.reset(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	newInfo->m_view.setViewport(rect);
	m_games.push_back(newInfo);
	
	return newInfo;
}

extern GameManager g_clientGame;