#include "stdafx.h"
#include "ClientGame.h"
#include "CombatComponent.h"
#include "CombatUtil.h"
#include "Tetris.h"

void CombatUtil::Attack(GameManager& manager, Tetris* owner, s32 dmg)
{
	for (s32 i = 0; i < manager.m_games.size(); i++)
	{
		Tetris* current = manager.m_games[i]->m_game;
		if (current != owner)
		{
			CombatComponent* combatCmp = current->GetComponent<CombatComponent>();
			if (combatCmp)
			{
				DamageEvent newDamage;
				newDamage.instigator = owner;
				newDamage.dmg = dmg;
				combatCmp->m_damageQueue.push_back(newDamage);
			}
		}
	}
}
