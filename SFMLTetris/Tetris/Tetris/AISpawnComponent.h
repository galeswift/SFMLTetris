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
		sf::Vector2f spawnPos;
		sf::Vector2f spawnScale;

		SpawnInfo()
			: handle(0)
			, rowSize(10)
			, columnSize(20)
			, updateFrequency(00)
			, spawnPos(0,0)
			, spawnScale(1,1)
		{

		}
	};	

	AISpawnComponent(Tetris* tetris);
	~AISpawnComponent();
		
	GameHandle AddAI(const SpawnInfo& info);

	std::vector<SpawnInfo> m_spawnQueue;
};

