#pragma once

class Tetris;

class AIHeuristic
{
public:
	AIHeuristic(float scalar)
		: m_scalar(scalar)
	{

	}

	virtual float GetScore(Tetris* m_tetrisBoard) = 0;
	float m_scalar;
};

class AIHeuristic_AggregateHeight
{
public:
	virtual float GetScore(Tetris* m_tetrisBoard) { return 1.0f; }
};

class AIHeuristic_CompletedLines
{
public:
	virtual float GetScore(Tetris* m_tetrisBoard) { return 1.0f; }
};

class AIHeuristic_Holes
{
public:
	virtual float GetScore(Tetris* m_tetrisBoard) { return 1.0f; }
};

class AIHeuristic_Bumpiness
{
public:
	virtual float GetScore(Tetris* m_tetrisBoard) { return 1.0f; }
};

class AIEvaluator
{
public:
	AIEvaluator(Tetris* m_tetrisBoard);
	void Update(float dt);

private:
	Tetris* m_tetrisBoard;
	std::vector<AIHeuristic*> m_heuristics;
};