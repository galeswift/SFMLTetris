#include "stdafx.h"
#include "ClientGame.h"
#include "CombatComponent.h"
#include "CombatUtil.h"
#include "Tetris.h"

void CombatUtil::Attack(Tetris * owner, s32 dmg)
{
	CombatComponent* combatCmp = owner->GetComponent<CombatComponent>();
	
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		if (current != owner)
		{
			CombatComponent* combatCmp = current->GetComponent<CombatComponent>();
			DamageEvent newDamage;
			newDamage.instigator = owner;
			newDamage.dmg = dmg;
			combatCmp->m_damageQueue.push_back(newDamage);
		}
		
	}
}
