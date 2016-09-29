#pragma once

#include "AICommon.h"
#include "System.h"
#include <string>

#define NUM_LOOKAHEAD (2)
#define AI_UPDATE_RATE_SECONDS (0.0f)
class Tetris;

class AIDebug
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

class AIHeuristic_GameLoss: public AIHeuristic
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

class AIEvaluator : public System
{
public:
	AIEvaluator(Tetris* tetrisBoard);
	void FindBestMove();
	bool CanFindMove() { return m_timeSinceLastUpdate > AI_UPDATE_RATE_SECONDS; }
	void Update(float dt);
	DesiredMoveSet GetBestMove() { return m_bestMoves[0]; }
	std::vector<AIDebug> m_debugHeuristics;
private:
	DesiredMoveSet _FindBestMove(Tetris* tetrisBoard, int lookaheads, bool holdPiece);
	Tetris* m_tetrisBoard;
	std::vector<AIHeuristic*> m_heuristics;	
	float m_timeSinceLastUpdate;
	DesiredMoveSet m_bestMoves[NUM_LOOKAHEAD];
};