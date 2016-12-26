#include "stdafx.h"
#include "AIControllerComponent.h"
#include "AIEvaluatorComponent.h"
#include "AISpawnComponent.h"
#include "AISpawnSystem.h"
#include "CombatComponent.h"
#include "Tetris.h"

AISpawnSystem::AISpawnSystem()
{
}


AISpawnSystem::~AISpawnSystem()
{
}

void AISpawnSystem::Update(float dt)
{
	for (s32 i = 0; i < g_clientGame.GetSpawnComponent()->m_spawnQueue.size(); i++)
	{
		AISpawnComponent::SpawnInfo& currentInfo = g_clientGame.GetSpawnComponent()->m_spawnQueue.at(i);
		Tetris* otherGame = new Tetris();				
		otherGame->Init(false, currentInfo.rowSize, currentInfo.columnSize);
		GameInfo* aiGame = g_clientGame.AddGame<GameInfo>(otherGame, sf::FloatRect(currentInfo.spawnPos.x/ WINDOW_WIDTH, currentInfo.spawnPos.y/ WINDOW_HEIGHT, currentInfo.spawnScale.x, currentInfo.spawnScale.y), currentInfo.handle);

		AIControllerComponent* aiComponent = new AIControllerComponent(otherGame);
		aiComponent->SetUpdateFrequency(currentInfo.updateFrequency);

		otherGame->m_components.push_back(aiComponent);
		otherGame->m_components.push_back(new AIEvaluatorComponent(otherGame));
		otherGame->m_components.push_back(new CombatComponent(otherGame));		
	}
	
	g_clientGame.GetSpawnComponent()->m_spawnQueue.clear();
}
