#include "stdafx.h"

#include "AIControllerSystem.h"
#include "AIEvaluatorSystem.h"
#include "AIMoveSystem.h"
#include "AISpawnComponent.h"
#include "AISpawnSystem.h"
#include "ClientGame.h"
#include "CombatComponent.h"
#include "CombatSystem.h"
#include "PlayerComponent.h"
#include "SoundComponent.h"
#include "SoundSystem.h"
#include "Tetris.h"

int main(int argc, char** argv)
{
	srand(unsigned int(time(NULL)));

	sf::ContextSettings windowSettings;
	windowSettings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML TETRIS", sf::Style::Default, windowSettings);

	GameManager gameManager;

	Tetris* localGame = new Tetris();
	localGame->Init(&gameManager, true, NUM_ROWS, NUM_COLS);
	gameManager.AddGame<GameInfo>(localGame, sf::FloatRect(0, 0, PLAYER_SCALE, PLAYER_SCALE), gameManager.ReserveHandle());

	// Components
	gameManager.AddComponent<PlayerComponent>(localGame);
	gameManager.AddComponent<SoundComponent>(localGame);

	CombatComponent* playerCombat = gameManager.AddComponent<CombatComponent>(localGame);
	playerCombat->m_MP = 10;
	playerCombat->m_attack = 2;
	playerCombat->m_defense = 1;

	AISpawnComponent* aiSpawner = gameManager.AddComponent<AISpawnComponent>(localGame);
	aiSpawner->Init(localGame);

	// Systems
	gameManager.m_systems.push_back(new AIEvaluatorSystem());
	gameManager.m_systems.push_back(new AIControllerSystem());
	gameManager.m_systems.push_back(new AIMoveSystem());
	gameManager.m_systems.push_back(new AISpawnSystem());
	gameManager.m_systems.push_back(new CombatSystem());
	gameManager.m_systems.push_back(new SoundSystem());

	std::vector<GameHandle> aiInfo;
	bool paused = false;
	sf::Clock clock;
	while (window.isOpen() && gameManager.IsRunning())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::BackSpace)
				{
					for (s32 i = 0; i < gameManager.m_games.size(); i++)
					{
						gameManager.m_games.at(i)->m_game->ApplyInput(INPUT_GARBAGE);
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
						aiSpawner->RemoveAI(gameManager, aiInfo.back());
						aiInfo.pop_back();
					}
				}
				else if (event.key.code == sf::Keyboard::S)
				{
					float flAIRandomness = 0.0f;
					int nRandRowRange = rand() % 14;
					int nRandColRange = rand() % 6;
					aiInfo.push_back(aiSpawner->AddAI(gameManager, NUM_ROWS - nRandRowRange, NUM_COLS - nRandColRange, 0.1 /*0.25f * rand()/ (float)RAND_MAX + 0.05)*/, sf::Vector2f(1.0f - flAIRandomness, 1.0f + flAIRandomness)));
				}
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Time time = clock.restart();
		if (!paused)
		{
			gameManager.Update(time.asSeconds());
		}
		gameManager.Draw(&window, time.asSeconds());
		window.display();
	}

	return 0;
}
