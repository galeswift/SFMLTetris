#pragma once
#include <vector>
#include "Constants.h"

class Tetris;
class AIEvaluator;
class AIController;
class System;

struct GameInfo
{
	GameInfo()
		: m_game(nullptr)
	{

	}

	~GameInfo()
	{
		for (int i = 0; i < m_systems.size(); i++)
		{
			delete m_systems[i];
			m_systems.erase(m_systems.begin() + i);
			i--;
		}
	}

	virtual void Update(float dt);	
	virtual void Draw(sf::RenderWindow* window, float dt);
	Tetris* m_game;
	std::vector<System*> m_systems;
	sf::View m_view;
};

struct AIGameInfo : public GameInfo
{
public:
	virtual void Update(float dt);	
	virtual void Draw(sf::RenderWindow* window, float dt);
	AIEvaluator* m_evaluator;
	AIController* m_controller;
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

