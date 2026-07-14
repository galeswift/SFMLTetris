#include "stdafx.h"
#include "AIControllerComponent.h"
#include "AIEvaluatorComponent.h"
#include "AISpawnComponent.h"
#include "AISpawnSystem.h"
#include "ClientGame.h"
#include "CombatComponent.h"
#include "Tetris.h"

void AISpawnSystem::Update(GameManager& manager, float dt)
{
	for (ComponentIterator itr = manager.GetComponents(COMPONENT_AI_SPAWN); itr.Get() != nullptr; itr++)
	{
		AISpawnComponent& spawner = *itr.Get<AISpawnComponent>();
		for (s32 i = 0; i < spawner.m_spawnQueue.size(); i++)
		{
			AISpawnComponent::SpawnInfo& currentInfo = spawner.m_spawnQueue.at(i);
			Tetris* otherGame = new Tetris();
			otherGame->Init(&manager, false, currentInfo.rowSize, currentInfo.columnSize);
			manager.AddGame<GameInfo>(otherGame, sf::FloatRect(currentInfo.spawnPos.x, currentInfo.spawnPos.y, currentInfo.spawnScale.x, currentInfo.spawnScale.y), currentInfo.handle);

			AIControllerComponent* aiComponent = manager.AddComponent<AIControllerComponent>(otherGame);
			aiComponent->SetUpdateFrequency(currentInfo.updateFrequency);

			AIEvaluatorComponent* evalComponent = manager.AddComponent<AIEvaluatorComponent>(otherGame);
			evalComponent->m_aiHeuristicRange = currentInfo.aiHeursticRange;

			manager.AddComponent<CombatComponent>(otherGame);

			spawner.m_currentSpawns.push_back(currentInfo);
		}

		spawner.m_spawnQueue.clear();
	}
}
