#pragma once
#include "System.h"

class AIMoveSystem : public System
{
public:
	void Update(GameManager& manager, float dt) override;
	void Draw(GameManager& manager, sf::RenderWindow* window, float dt) override;
};
