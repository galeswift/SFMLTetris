#include "stdafx.h"
#include "AISpawnComponent.h"
#include "ClientGame.h"
#include "ComponentPool.h"
#include "Tetris.h"

DEFINE_COMPONENT(AISpawnComponent)

AISpawnComponent::AISpawnComponent()
	: m_aiSpawnPos(0,0)
	, m_flScale(0.5f)
	, m_flXOffset(0.0f)
	, m_flYOffset(0.0f)
{
}

void AISpawnComponent::Reset()
{
	m_currentSpawns.clear();
	m_spawnQueue.clear();
	m_aiSpawnPos = sf::Vector2f(0, 0);
	m_flScale = 0.5f;
	m_flXOffset = 0.0f;
	m_flYOffset = 0.0f;
}

void AISpawnComponent::Init(Tetris* playerGame)
{
	m_owner = playerGame;
	m_aiSpawnPos.x = playerGame->GetTotalGameWidth();
	m_aiSpawnPos.y = 0;

	m_flScale = 0.5f;
	m_flXOffset = playerGame->GetTotalGameWidth() * m_flScale;
	m_flYOffset = playerGame->GetTotalGameHeight() * m_flScale + 50;
}

void AISpawnComponent::RemoveAI(GameManager& manager, GameHandle handle)
{
	for (s32 i = 0; i < m_currentSpawns.size(); i++)
	{
		if (m_currentSpawns[i].handle == handle)
		{
			manager.RemoveGame(handle);
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

GameHandle AISpawnComponent::AddAI(GameManager& manager, s32 rows, s32 cols, float flUpdateFrequency, sf::Vector2f flAIHeuristicRange)
{
	SpawnInfo newAI;
	newAI.handle = manager.ReserveHandle();

	newAI.rowSize = rows;
	newAI.columnSize = cols;
	newAI.spawnPos = m_aiSpawnPos;
	newAI.spawnScale = sf::Vector2f(m_flScale, m_flScale);
	newAI.aiHeursticRange = flAIHeuristicRange;
	newAI.updateFrequency = flUpdateFrequency;
	m_spawnQueue.push_back(newAI);

	m_aiSpawnPos.x += m_flXOffset;

	if (m_aiSpawnPos.x >= WINDOW_WIDTH - m_flXOffset)
	{
		m_aiSpawnPos.x = m_owner->GetTotalGameWidth();
		m_aiSpawnPos.y += m_flYOffset;
	}

	return newAI.handle;
}
