#pragma once

class Tetris;
class Component
{
public: 
	Component(Tetris* owner);
	virtual ~Component() {};
	Tetris* m_owner;
};
