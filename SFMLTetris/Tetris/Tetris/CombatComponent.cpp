#include "stdafx.h"
#include "CombatComponent.h"

CombatComponent::CombatComponent(Tetris* owner)
	: Component(owner)
	, m_attack(0)
	, m_defense(0)
	, m_special(0)
	, m_HP(0)
	, m_MP(0)
{
}


CombatComponent::~CombatComponent()
{
}
