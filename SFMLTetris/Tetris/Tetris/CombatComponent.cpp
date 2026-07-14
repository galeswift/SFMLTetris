#include "stdafx.h"
#include "CombatComponent.h"
#include "ComponentPool.h"

DEFINE_COMPONENT(CombatComponent)

CombatComponent::CombatComponent()
{
	Reset();
}

void CombatComponent::Reset()
{
	m_attackingTarget = NULL;
	m_attack = 0;
	m_defense = 0;
	m_special = 0;
	m_MP = 0;
	m_damageQueue.clear();
}
