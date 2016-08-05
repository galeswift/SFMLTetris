#pragma once

// Controls the movement of a piece based on a desired position
class AIController
{

public:
	AIController(Tetris* m_tetrisBoard);
	void Update(float dt);

private:
	Tetris* m_tetrisBoard;
};
