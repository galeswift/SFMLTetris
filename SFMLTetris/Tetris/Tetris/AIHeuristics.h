#pragma once

class Tetris;

class AIDebugHeuristic
{
public:
	float m_lastScore;
	std::string m_description;
};

class AIHeuristic
{
public:
	AIHeuristic(float scalar)
		: m_scalar(scalar)
	{

	}

	virtual float GetScore(const Tetris* original, Tetris* newTetrisBoard) = 0;
	float m_scalar;
};

class AIHeuristic_AggregateHeight : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};

class AIHeuristic_HighestCol : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};

class AIHeuristic_GameLoss : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};

class AIHeuristic_CompletedLines : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};

class AIHeuristic_Holes : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};


class AIHeuristic_Blockade : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};

class AIHeuristic_Bumpiness : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const Tetris* original, Tetris* tetrisBoard);
};
