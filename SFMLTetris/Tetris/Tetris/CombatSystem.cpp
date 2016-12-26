#include "stdafx.h"

#include "CombatComponent.h"
#include "CombatSystem.h"
#include "Tetris.h"

void CombatSystem::Update(float dt)
{
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		CombatComponent* combatCmp = current->GetComponent<CombatComponent>();		
		if (combatCmp)
		{
			for (auto dmgIterator = combatCmp->m_damageQueue.begin(); dmgIterator != combatCmp->m_damageQueue.end(); )
			{
				combatCmp->m_owner->AddGarbage(dmgIterator->dmg);
				dmgIterator = combatCmp->m_damageQueue.erase(dmgIterator);
			}

		}
	}	
}
