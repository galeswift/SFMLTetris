#include "stdafx.h"
#include "AICommon.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorUtil.h"
#include "AIHeuristics.h"
#include "ClientGame.h"
#include "Tetris.h"


AIEvaluatorUtil::AIEvaluatorUtil()
{
}


AIEvaluatorUtil::~AIEvaluatorUtil()
{
}

void AIEvaluatorUtil::FindBestMove(AIEvaluatorComponent* comp)
{
	if (comp)
	{
		comp->m_timeSinceLastUpdate = 0.0f;

		// Stop AI when game is lost
		comp->m_bestMoves[0] = __FindBestMove(comp->m_owner, comp, NUM_LOOKAHEAD, false);
		DesiredMoveSet heldMove = __FindBestMove(comp->m_owner, comp, NUM_LOOKAHEAD, true);

		if (heldMove.score > comp->m_bestMoves[0].score)
		{
			comp->m_bestMoves[0] = heldMove;
		}
	}
}

DesiredMoveSet AIEvaluatorUtil::__FindBestMove(Tetris * tetrisBoard, AIEvaluatorComponent * ownerComp, int numLookaheads, bool holdPiece)
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

			if (holdPiece)
			{
				boardCopy.KeySwap();
				result.swapPiece = true;
			}

			for (int numRotations = 0; numRotations < j; numRotations++)
			{
				boardCopy.m_currentPiece->Rotate(true);
			}
			boardCopy.m_currentPiece->Move(-tetrisBoard->m_cols, 0);
			boardCopy.m_currentPiece->Move(i, 0);
			int colIdx = (int)boardCopy.m_currentPiece->m_originColIdx;
			boardCopy.DropCurrentPiece();

			// Try the current piece in a specific position
			// Try the next piece in a specific position	

			// Try the next lookahead
			float currentScore = 0.0f;
			for (auto h : ownerComp->m_heuristics)
			{
				// Score the grid	
				float score = h->GetScore(ownerComp->m_owner, &boardCopy);
				currentScore += score;
			}

			DesiredMoveSet lookaheadMove;
			if (numLookaheads > 0)
			{
				lookaheadMove = __FindBestMove(&boardCopy, ownerComp, numLookaheads, false);
				currentScore += lookaheadMove.score;
			}

			if (currentScore > result.score)
			{
				result.score = currentScore;
				result.numRotations = j;
				result.col = colIdx;
				result.id = ownerComp->m_owner->m_currentPiece->m_id;

				int index = 0;

				for (auto h : ownerComp->m_heuristics)
				{
					if (numLookaheads == NUM_LOOKAHEAD - 1)
					{
						float score = h->GetScore(ownerComp->m_owner, &boardCopy);
						ownerComp->m_debugHeuristics[index].m_lastScore = score;
					}
					index++;
				}

				if (numLookaheads > 0)
				{
					ownerComp->m_bestMoves[NUM_LOOKAHEAD - numLookaheads] = lookaheadMove;
				}
			}
		}
	}

	return result;
}
