#include "stdafx.h"
#include "AISpawnComponent.h"
#include "ClientGame.h"

AISpawnComponent::AISpawnComponent(Tetris * tetris)
	: Component(tetris)	
{
}

AISpawnComponent::~AISpawnComponent()
{
}
GameHandle AISpawnComponent::AddAI(const SpawnInfo& info)
{
	GameHandle newHandle = g_clientGame.ReserveHandle();
	m_spawnQueue.push_back(info);
	m_spawnQueue.back().handle = newHandle;	
	return newHandle;
}
