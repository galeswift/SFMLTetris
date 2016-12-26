#include "stdafx.h"
#include "AISpawnComponent.h"
#include "ClientGame.h"

AISpawnComponent::AISpawnComponent(Tetris * tetris)
	: Component(tetris)	
	, m_aiSpawnPos(350,0)
{
}

AISpawnComponent::~AISpawnComponent()
{
}
void AISpawnComponent::RemoveAI(GameHandle handle)
{
	for (s32 i = 0; i < m_currentSpawns.size(); i++)
	{
		if (m_currentSpawns[i].handle == handle)
		{
			g_clientGame.RemoveGame(handle);
			m_aiSpawnPos = m_currentSpawns[i].spawnPos;
			m_currentSpawns.erase(m_currentSpawns.begin() + i);
			break;
		}
	}
}

GameHandle AISpawnComponent::AddAI(s32 rows, s32 cols)
{		
	SpawnInfo newAI;
	newAI.handle = g_clientGame.ReserveHandle();

	newAI.rowSize = rows;
	newAI.columnSize = cols;
	newAI.spawnPos = m_aiSpawnPos;
	newAI.spawnScale = sf::Vector2f(0.5, 0.5);
	newAI.updateFrequency = .2f;	
	m_aiSpawnPos.x += 350;
	if (m_aiSpawnPos.x > WINDOW_WIDTH - 250)
	{
		m_aiSpawnPos.x = 350;
		m_aiSpawnPos.y += 300;
	}

	m_spawnQueue.push_back(newAI);	
	return newAI.handle;
}
