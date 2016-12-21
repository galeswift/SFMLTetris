#pragma once
#include "System.h"
class AIMoveSystem : public System
{
public:
	AIMoveSystem();
	~AIMoveSystem();
	void Update(float dt) override;	
	void Draw(sf::RenderWindow* window, float dt) override;
};

