#pragma once

class GameManager;

class System
{
public:
	virtual ~System() {};
	virtual void Update(GameManager& manager, float dt) = 0;
	virtual void Draw(GameManager& manager, sf::RenderWindow* window, float dt) {};
};
