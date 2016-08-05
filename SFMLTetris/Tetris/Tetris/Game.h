#pragma once
#include <vector>

class Tetris;

struct GameInfo
{
	Tetris* game;
	sf::View view;
};
class GameManager
{
public:
	~GameManager();
	void AddGame(Tetris* game, const sf::FloatRect& rect);
	void Update(sf::RenderWindow* window, float dt);
	bool IsRunning();

	std::vector<GameInfo> m_games;
};