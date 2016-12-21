#pragma once
#include "Component.h"
class Tetris;

class CombatComponent :	public Component
{
public:
	CombatComponent(Tetris* owner);
	virtual ~CombatComponent();

	Tetris* m_attackingTarget;
	int m_attack;
	int m_defense;
	int m_special;
	int m_HP;
	int m_MP;
};

