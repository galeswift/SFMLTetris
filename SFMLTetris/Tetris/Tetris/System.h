#pragma once

#include "ClientGame.h"

class System
{
public:
	virtual void Update(float dt) = 0;
	virtual void Draw(sf::RenderWindow* window, float dt) {};
};
