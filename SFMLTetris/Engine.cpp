// Engine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


int main()
{
	sf::RenderWindow window(sf::VideoMode(1024, 512), "Welcome to SDVA 203!");
	sf::CircleShape shape(10.f);
	
	shape.setOutlineColor(sf::Color::Red);
	shape.setOutlineThickness(2.0f);
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setPosition(512, 200);
	
	sf::RectangleShape Troll(sf::Vector2f(50, 50));
	Troll.setPosition(100, 100);

	sf::RectangleShape leftPaddle(sf::Vector2f(20, 100));
	leftPaddle.setFillColor(sf::Color(200, 50, 50));
	leftPaddle.setOutlineColor(sf::Color::Blue);
	leftPaddle.setOutlineThickness(2.0f);
	leftPaddle.setPosition(40, 256 - 50);

	sf::RectangleShape rightPaddle(sf::Vector2f(20, 100));
	rightPaddle.setFillColor(sf::Color(50, 50, 200));
	rightPaddle.setOutlineColor(sf::Color::Red);
	rightPaddle.setOutlineThickness(2.0f);
	rightPaddle.setPosition(1024 - 20 - 20, 256 - 50);

	sf::Clock clock;

	sf::Vector2f velocity;
	velocity.x = 200;
	velocity.y = 200;
	float shotPower = 0;
	bool isCharging = false;
	bool restart = false;
	bool move = false;
	int counter = 1;
	int counter2 = 1;
	
	//---------------------------------------------------------------------
	sf::Music overtimeMusic;
	overtimeMusic.openFromFile("Audio/testSong.ogg");
	overtimeMusic.play();

	sf::Music wallSound;
	wallSound.openFromFile("Audio/Jump.wav");
	wallSound.play();

	//--------------------------------------------------------------------
		
	




	sf::Font Score1;
	sf::Font Score2;
	sf::Font Team1;
	sf::Font Team2;
	sf::Text player1;
	sf::Text player2;
	sf::Text Point1;
	sf::Text Point2;

	if (!Score1.loadFromFile("Fonts/Sweet Sorrow.ttf"))
	{
		// error...
	}
	if (!Score2.loadFromFile("Fonts/Sweet Sorrow.ttf"))
	{
		// error...
	}
	if (!Team1.loadFromFile("Fonts/Sweet Sorrow.ttf"))
	{
		// error...
	}
	if (!Team2.loadFromFile("Fonts/Sweet Sorrow.ttf"))
	{
		// error...
	}

	sf::Sprite myTroll;
	sf::Texture texture;
	if (!texture.loadFromFile("Texture/Troll.jpg"))
	{
		// error...
	}

	myTroll.setTexture(texture);
	myTroll.setPosition(1024 / 2, 512 / 2);
	myTroll.setOrigin(190,190);

	player1.setFont(Score1);
	player1.setString("Player 1 : ");
	player1.setCharacterSize(50);
	player1.setColor(sf::Color::Red);
	player1.setPosition(150, 5);

	Point1.setFont(Team1);
	Point1.setCharacterSize(35);
	Point1.setColor(sf::Color::Red);
	Point1.setPosition(420, 5);

	player2.setFont(Score2);
	player2.setString("Player 2 : ");
	player2.setCharacterSize(50);
	player2.setColor(sf::Color::Blue);
	player2.setPosition(600, 5);

	Point2.setFont(Team2);
	Point2.setCharacterSize(35);
	Point2.setColor(sf::Color::Blue);
	Point2.setPosition(880, 5);

	while (window.isOpen())
	{
		sf::Time deltaTime = clock.restart();
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{

				if (event.key.code == sf::Keyboard::P)
				{
					if (overtimeMusic.getStatus() == sf::SoundSource::Playing)
					{
						overtimeMusic.pause();
					}
					else
					{
						overtimeMusic.play();
					}
				}
			}
		}



		shape.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));

		bool paddleMoved = false;
		bool moveUp = false;
		float middlePos = rightPaddle.getPosition().y + rightPaddle.getSize().y * 0.5f;
		if (shape.getPosition().y < middlePos)
		{
			moveUp = true;
		}

		if (moveUp)
		{
			paddleMoved = true;
			rightPaddle.move(sf::Vector2f(0, -1024 * deltaTime.asSeconds()));
		}
		else
		{
			paddleMoved = true;
			rightPaddle.move(sf::Vector2f(0, 1024 * deltaTime.asSeconds()));
		}


		// Move left paddle up

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			leftPaddle.move(0, -300 * deltaTime.asSeconds());
			move = true;
		}
		// Move left paddle down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			leftPaddle.move(0, 300 * deltaTime.asSeconds());
			move = true;
		}
		// Move right paddle up
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			rightPaddle.move(0, -300 * deltaTime.asSeconds());
			move = true;
		}
		// Move right paddle down
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			rightPaddle.move(0, 300 * deltaTime.asSeconds());
			move = true;
		}
		// Constrain right paddle to edges of screen
		if (rightPaddle.getPosition().y < 0)
		{
			rightPaddle.setPosition(rightPaddle.getPosition().x, 0);
		}
		else if (rightPaddle.getPosition().y + rightPaddle.getSize().y > 512)
		{
			rightPaddle.setPosition(rightPaddle.getPosition().x, 512 - rightPaddle.getSize().y);
		}

		// Constrain left paddle to edges of screen
		if (leftPaddle.getPosition().y < 0)
		{
			leftPaddle.setPosition(leftPaddle.getPosition().x, 0);
		}
		else if (leftPaddle.getPosition().y + leftPaddle.getSize().y > 512)
		{
			leftPaddle.setPosition(leftPaddle.getPosition().x, 512 - leftPaddle.getSize().y);
		}

		sf::Vector2f ballPos = shape.getPosition();
		float radius = shape.getRadius();
		if (ballPos.x + radius > leftPaddle.getPosition().x	&&
			ballPos.x - radius < leftPaddle.getPosition().x + leftPaddle.getSize().x &&
			ballPos.y + radius > leftPaddle.getPosition().y	&&
			ballPos.y - radius < leftPaddle.getPosition().y + leftPaddle.getSize().y)
		{
			shape.setPosition(leftPaddle.getPosition().x + 100 + shape.getRadius(), shape.getPosition().y);
			velocity.x *= -1.05;
		}
		if (ballPos.x + radius > rightPaddle.getPosition().x	&&
			ballPos.x - radius < rightPaddle.getPosition().x + rightPaddle.getSize().x &&
			ballPos.y + radius > rightPaddle.getPosition().y	&&
			ballPos.y + radius < rightPaddle.getPosition().y + rightPaddle.getSize().y)
		{
			shape.setPosition(rightPaddle.getPosition().x - shape.getRadius(), shape.getPosition().y);
			velocity.x *= -1.05;
		}

		// Ball bouncing off the top wall
		if (shape.getPosition().y < shape.getRadius())
		{
			shape.setPosition(shape.getPosition().x, shape.getRadius());
			velocity.y *= -1;
			wallSound.play();
		}
		// Ball bouncing off the bottom wall
		else if (shape.getPosition().y + shape.getRadius() > 512)
		{
			shape.setPosition(shape.getPosition().x, 512 - shape.getRadius());
			velocity.y *= -1;
			wallSound.play();
		}
		// Ball bouncing off the left wall 
		else if (shape.getPosition().x <= shape.getRadius())
		{
			shape.setPosition(1024 / 2, 512 / 2);
			move = false;
		}
		//right wall
		else if (shape.getPosition().x >= 1024 - shape.getRadius())
		{
			shape.setPosition(1024 / 2, 512 / 2);
			move = false;

		}


		if (move)
		{
			shape.move(velocity.x * deltaTime.asSeconds(), velocity.y * deltaTime.asSeconds());
		}



		std::ostringstream Number1;
		std::ostringstream Number2;

		//left score

		if ((shape.getPosition().x >= 1024 - shape.getRadius()))
		{
			Number1 << counter;
			Point1.setString(Number1.str());
			counter++;
		}
		//right score
		if (shape.getPosition().x <= shape.getRadius())
		{
			Number2 << counter2;
			Point2.setString(Number2.str());
			counter2++;
		}




		window.clear();
		window.draw(shape);
		window.draw(player1);
		window.draw(player2);
		window.draw(Point1);
		window.draw(Point2);
		if (counter == 6 || counter2 == 6)
		{
			window.draw(myTroll);
		}
		window.draw(leftPaddle);
		window.draw(rightPaddle);


		
		window.display();
	}

	return 0;
}
