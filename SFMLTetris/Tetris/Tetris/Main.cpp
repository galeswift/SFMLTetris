#include "stdafx.h"

#include "AIControllerComponent.h"
#include "AIControllerSystem.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorSystem.h"
#include "AIMoveSystem.h"
#include "AISpawnComponent.h"
#include "AISpawnSystem.h"
#include "ClientGame.h"
#include "CombatComponent.h"
#include "CombatSystem.h"
#include "ClientGame.h"
#include "Networking.h"
#include "PlayerComponent.h"
#include "Tetris.h"

TetrisNetworkBase* g_networkPtr;

int main(int argc, char** argv)
{
	srand(unsigned int(time(NULL)));
	std::string ipAddress("");
	if (argc > 1)
	{
		if (_stricmp(argv[1], "--server") == 0)
		{
			g_networkPtr = new TetrisServer();
		}
		else
		{
			ipAddress = argv[1];
			g_networkPtr = new TetrisClient(ipAddress);
		}
	}

	if (g_networkPtr != NULL)
	{
		if (!g_networkPtr->Init())
		{
			delete g_networkPtr;
			g_networkPtr = NULL;
		}
	}

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML TETRIS");
	
	sf::View playerView;

	Tetris* localGame = new Tetris();
	localGame->Init(true, NUM_ROWS, NUM_COLS);
	localGame->m_components.push_back(new PlayerComponent(localGame));

	CombatComponent* playerCombat = new CombatComponent(localGame);
	localGame->m_components.push_back(playerCombat);	
	playerCombat->m_MP = 10;
	playerCombat->m_attack = 2;
	playerCombat->m_defense = 1;		
	
	GameInfo* playerGame = g_clientGame.AddGame<GameInfo>(localGame, sf::FloatRect(0, 0, .5, .5), g_clientGame.ReserveHandle());

	// Components
	g_clientGame.m_components.push_back(new AISpawnComponent(NULL));

	// systems
	AIEvaluatorSystem* aiPlayer = new AIEvaluatorSystem();
	AIControllerSystem* aiController = new AIControllerSystem();	
	g_clientGame.m_systems.push_back(aiPlayer);
	g_clientGame.m_systems.push_back(aiController);
	g_clientGame.m_systems.push_back(new AIMoveSystem());
	g_clientGame.m_systems.push_back(new AISpawnSystem());	
	g_clientGame.m_systems.push_back(new CombatSystem());
		
	std::vector<GameHandle> aiInfo;
	bool paused = false;
	sf::Clock clock;
	while (window.isOpen() && g_clientGame.IsRunning())
	{
		if (g_networkPtr != NULL)
		{
			g_networkPtr->Loop();
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::BackSpace)
				{
					for (int i = 0; i < g_clientGame.m_games.size(); i++)
					{
						g_clientGame.m_games.at(i)->m_game->KeyGarbage();
					}
				}
				else if (event.key.code == sf::Keyboard::P)
				{
					paused = !paused;
				}
				else if (event.key.code == sf::Keyboard::D)
				{					
					if (aiInfo.size() > 0)
					{						
						AISpawnComponent* spawnComp = g_clientGame.GetSpawnComponent();
						spawnComp->RemoveAI(aiInfo.back());						
						aiInfo.pop_back();
					}
				}
				else if (event.key.code == sf::Keyboard::S)
				{									
					AISpawnComponent* spawnComp = g_clientGame.GetSpawnComponent();
					float flAIRandomness = 0.0f;
					int nRandRowRange = rand() % 14;
					int nRandColRange = rand() % 6;
					aiInfo.push_back(spawnComp->AddAI(NUM_ROWS - nRandRowRange, NUM_COLS - nRandColRange, 0.1 /*0.25f * rand()/ (float)RAND_MAX + 0.05)*/, sf::Vector2f(1.0f - flAIRandomness, 1.0f + flAIRandomness)));
				}
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		sf::Time time = clock.restart();
		if (!paused)
		{
			g_clientGame.Update(time.asSeconds());
		}
		g_clientGame.Draw(&window, time.asSeconds());
		window.display();
	}

	if (g_networkPtr != NULL)
	{
		delete g_networkPtr;
		g_networkPtr = NULL;
	}
	return 0;
}
