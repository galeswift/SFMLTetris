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
	void RemoveAI(GameHandle handle);
	GameHandle AddAI(s32 rows, s32 cols);

	sf::Vector2f m_aiSpawnPos;
	std::vector<SpawnInfo> m_currentSpawns;
	std::vector<SpawnInfo> m_spawnQueue;
};

