#include "stdafx.h"

#include "ClientGame.h"
#include "CombatComponent.h"
#include "CombatSystem.h"
#include "Tetris.h"

void CombatSystem::Update(GameManager& manager, float dt)
{
	for (ComponentIterator itr = manager.GetComponents(COMPONENT_COMBAT); itr.Get() != nullptr; itr++)
	{
		CombatComponent* combatCmp = itr.Get<CombatComponent>();
		for (auto dmgIterator = combatCmp->m_damageQueue.begin(); dmgIterator != combatCmp->m_damageQueue.end(); )
		{
			combatCmp->m_owner->AddGarbage(dmgIterator->dmg);
			dmgIterator = combatCmp->m_damageQueue.erase(dmgIterator);
		}
	}
}
