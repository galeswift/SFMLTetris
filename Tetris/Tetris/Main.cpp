#include "stdafx.h"

#include "Game.h"
#include "Tetris.h"
#include "Networking.h"

TetrisNetworkBase* g_networkPtr;

int main(int argc, char** argv)
{

	std::string ipAddress("");
	if (argc > 1)
	{
		if (stricmp(argv[1], "--server") == 0)
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
	localGame->Init();
	gameMgr.AddGame(localGame, sf::FloatRect(.5, 0, 0.4, .4));

	Tetris* otherGame = new Tetris();
	otherGame->Init();
	gameMgr.AddGame(otherGame, sf::FloatRect(0.5, 0.5, .4, 0.4));
	
	
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
			if (event.type == sf::Event::Closed)
				window.close();
		}
		
		sf::Time time = clock.restart();
		gameMgr.Update(&window, time.asSeconds());
				
		//window.setView(window.getDefaultView());
		//window.clear(sf::Color(40, 40, 40));
		//game.Draw(&window);
		
		window.display();
	}

	if (g_networkPtr != NULL)
	{
		delete g_networkPtr;
		g_networkPtr = NULL;
	}
	return 0;
}
