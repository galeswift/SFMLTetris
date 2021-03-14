#include "stdafx.h"
#include "AISpawnComponent.h"
#include "ClientGame.h"

AISpawnComponent::AISpawnComponent(Tetris * tetris)
	: Component(tetris)	
	, m_aiSpawnPos(0,0)
{
	m_flScale = 0.5f;
	m_flXOffset = FIELD_ORIGIN_X + GRID_SIZE * NUM_COLS * m_flScale;
	m_flYOffset = GRID_SIZE * NUM_ROWS * m_flScale * 1.1;
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

	m_aiSpawnPos.x -= m_flXOffset;

	if (m_aiSpawnPos.x <= FIELD_ORIGIN_X)
	{
		m_aiSpawnPos.x = WINDOW_WIDTH - m_flXOffset;
		m_aiSpawnPos.y -= m_flYOffset;
	}

}

GameHandle AISpawnComponent::AddAI(s32 rows, s32 cols, float flUpdateFrequency, sf::Vector2f flAIHeuristicRange)
{	
	m_aiSpawnPos.x += m_flXOffset;

	if (m_aiSpawnPos.x >= WINDOW_WIDTH - m_flXOffset)
	{
		m_aiSpawnPos.x = m_flXOffset;
		m_aiSpawnPos.y += m_flYOffset;
	}


	SpawnInfo newAI;
	newAI.handle = g_clientGame.ReserveHandle();

	newAI.rowSize = rows;
	newAI.columnSize = cols;
	newAI.spawnPos = m_aiSpawnPos;
	newAI.spawnScale = sf::Vector2f(m_flScale, m_flScale);
	newAI.aiHeursticRange = flAIHeuristicRange;
	newAI.updateFrequency = flUpdateFrequency;

	m_spawnQueue.push_back(newAI);	
	return newAI.handle;
}
