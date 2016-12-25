#pragma once

#include "Component.h"

struct GameInfo;

class AISpawnComponent : public Component
{
public:
	struct SpawnInfo
	{		
		GameHandle handle;
		s32 rowSize;
		s32 columnSize;
		f32 updateFrequency;
	};	

	AISpawnComponent(Tetris* tetris);
	~AISpawnComponent();
	
	GameHandle AddAI(const SpawnInfo& info);

	std::vector<SpawnInfo> m_spawnQueue;
};

