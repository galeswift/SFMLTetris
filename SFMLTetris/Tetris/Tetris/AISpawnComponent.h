#pragma once

#include "Component.h"

class GameInfo;


class AISpawnComponent : public Component
{
public:
	struct SpawnInfo
	{
		s32 rowSize;
		s32 columnSize;
		f32 updateFrequency;
	};

	using Component::Component;
	~AISpawnComponent();

	void AddAI(const SpawnInfo& info);

	std::vector<GameInfo*> m_currentGames;
	std::vector<SpawnInfo> m_spawnQueue;
};

