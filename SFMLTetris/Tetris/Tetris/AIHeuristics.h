#pragma once

class BoardState;

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
	virtual ~AIHeuristic() {};

	virtual float GetScore(const BoardState* original, BoardState* newBoard) = 0;
	float m_scalar;
};

class AIHeuristic_AggregateHeight : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};

class AIHeuristic_HighestCol : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};

class AIHeuristic_DeepestHole : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};

class AIHeuristic_GameLoss : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};

class AIHeuristic_CompletedLines : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};

class AIHeuristic_Holes : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};


class AIHeuristic_Blockade : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};

class AIHeuristic_Bumpiness : public AIHeuristic
{
public:
	using AIHeuristic::AIHeuristic;
	virtual float GetScore(const BoardState* original, BoardState* board);
};
