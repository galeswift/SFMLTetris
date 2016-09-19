#include "stdafx.h"

#include "AIController.h"
#include "AIEvaluator.h"
#include "Game.h"
#include "Tetris.h"
#include "Networking.h"

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
	GameManager gameMgr;
	Tetris* localGame = new Tetris();	
	localGame->Init(true, NUM_ROWS, NUM_COLS);
	gameMgr.AddGame<GameInfo>(localGame, sf::FloatRect(0, 0, .5, .5));


	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Tetris* otherGame = new Tetris();
			otherGame->Init(false, rand()%10 + 10, rand()% 5 + 5);
			AIEvaluator* aiPlayer1 = new AIEvaluator(otherGame);
			AIController* aiController1 = new AIController(otherGame);
			AIGameInfo* aiGame = gameMgr.AddGame<AIGameInfo>(otherGame, sf::FloatRect(0.5f + 0.25f * j, 0.25f * i, .35f, 0.35f));
			aiGame->m_systems.push_back(aiPlayer1);
			aiGame->m_systems.push_back(aiController1);
			aiGame->m_evaluator = aiPlayer1;
			aiGame->m_controller = aiController1;
		}
	}
	sf::Clock clock;

	while (window.isOpen() && gameMgr.IsRunning())
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
				if (event.key.code == sf::Keyboard::Tilde)
				{
					gameMgr.m_games.at(0)->m_systems.push_back(new AIEvaluator(localGame));
				}
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		sf::Time time = clock.restart();
		gameMgr.Update(&window, time.asSeconds());
				
		window.display();
	}

	if (g_networkPtr != NULL)
	{
		delete g_networkPtr;
		g_networkPtr = NULL;
	}
	return 0;
}
