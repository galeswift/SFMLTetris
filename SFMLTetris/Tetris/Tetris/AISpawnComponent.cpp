#include "stdafx.h"
#include "AISpawnComponent.h"

AISpawnComponent::~AISpawnComponent()
{
}

void AISpawnComponent::AddAI(const SpawnInfo & info)
{
	m_spawnQueue.push_back(info);
}
