#include "stdafx.h"

#include "AIEvaluator.h"
#include "Tetris.h"

AIEvaluator::AIEvaluator(Tetris* tetrisBoard)
	: m_tetrisBoard(tetrisBoard)
{
	// stagger updates
	m_timeSinceLastUpdate = AI_UPDATE_RATE_SECONDS + 0.5f *  (float)(rand()) / RAND_MAX;
	m_heuristics.push_back(new AIHeuristic_AggregateHeight(-.75));
//	m_heuristics.push_back(new AIHeuristic_CompletedLines(2));
	m_heuristics.push_back(new AIHeuristic_Holes(-5));
	m_heuristics.push_back(new AIHeuristic_Bumpiness(-0.5));

	m_debugHeuristics.push_back(AIDebug{0.0f, "Height"});
	//m_debugHeuristics.push_back(AIDebug{ 0.0f, "Completed Lines" });
	m_debugHeuristics.push_back(AIDebug{ 0.0f, "Holes" });
	m_debugHeuristics.push_back(AIDebug{ 0.0f, "Bumpiness" });
}

void AIEvaluator::Update(float dt)
{
	// Not using this right now
	m_timeSinceLastUpdate += dt;	
}


void AIEvaluator::FindBestMove()
{
	m_timeSinceLastUpdate = 0.0f;
	m_bestMoves[0] = _FindBestMove(m_tetrisBoard, NUM_LOOKAHEAD);	
}

DesiredMoveSet AIEvaluator::_FindBestMove(Tetris* tetrisBoard, int numLookaheads)
{
	DesiredMoveSet result;

	numLookaheads--;

	if (!tetrisBoard->m_currentPiece)
	{
		return result;
	}
	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		// Try all rotations	
		int rotations = tetrisBoard->m_currentPiece->MaxRotations();
		for (int j = 0; j < rotations; j++)
		{
			// Make a copy of the board
			Tetris boardCopy;
			boardCopy.Clone(tetrisBoard);

			for (int numRotations = 0; numRotations < j; numRotations++)
			{
				boardCopy.m_currentPiece->Rotate(true);
			}
			boardCopy.m_currentPiece->Move(-tetrisBoard->m_cols, 0);
			boardCopy.m_currentPiece->Move(i, 0);
			int colIdx = boardCopy.m_currentPiece->m_originColIdx;
			boardCopy.DropCurrentPiece();

			// Try the current piece in a specific position
			// Try the next piece in a specific position	

			// Try the next lookahead
			float currentScore = 0.0f;	
			for (auto h : m_heuristics)
			{
				// Score the grid	
				float score = h->GetScore(m_tetrisBoard, &boardCopy);
				currentScore += score;				
			}

			DesiredMoveSet lookaheadMove;
			if (numLookaheads > 0)
			{				
				lookaheadMove = _FindBestMove(&boardCopy, numLookaheads);
				currentScore += lookaheadMove.score;
			}

			if (currentScore > result.score)
			{
				result.score = currentScore;
				result.numRotations = j;
				result.col = colIdx;
				result.id = m_tetrisBoard->m_currentPiece->m_id;
				
				int index = 0;
				for (auto h : m_heuristics)
				{

					if (numLookaheads == NUM_LOOKAHEAD - 1)
					{
						float score = h->GetScore(m_tetrisBoard, &boardCopy);
						m_debugHeuristics[index].m_lastScore = score;
					}
					index++;
				}

				if (numLookaheads > 0)
				{
					m_bestMoves[NUM_LOOKAHEAD - numLookaheads] = lookaheadMove;
				}
			}
		}
	}

	return result;
}

float AIHeuristic_AggregateHeight::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;
	
	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			if (tetrisBoard->m_grid.m_cells[i][j].m_isFilled)
			{
				int height = tetrisBoard->m_rows - j;
				if (result < height)
				{
					result = height;
				}

				break;				
			}
		}
	}
	
	return m_scalar * result;
}

float AIHeuristic_CompletedLines::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = tetrisBoard->m_clearedRows - original->m_clearedRows;
	return m_scalar * result;
}

float AIHeuristic_Holes::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;

	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		bool bFoundBlock = false;
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{			
			GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][j];
			if (gridCell.m_isFilled)
			{
				bFoundBlock = true;
			}
			else if (bFoundBlock)
			{
				result++;
			}
		}
	}
	
	return m_scalar * result;
}

float AIHeuristic_Bumpiness::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;

	int prevHeight = 0;
	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{		
		int height = 0;
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][j];
			if (gridCell.m_isFilled)
			{				
				height = tetrisBoard->m_rows - j;
				break;
			}
		}

		if (i > 0)
		{
			result += abs(prevHeight - height);
		}

		prevHeight = height;
	}


	return m_scalar * result;
}
