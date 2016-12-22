#pragma once
#include "Component.h"
class Tetris;

struct DamageEvent
{
	Tetris* instigator;
	int dmg;
};

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

	std::vector<DamageEvent> m_damageQueue;
};

