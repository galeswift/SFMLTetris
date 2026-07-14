#include "stdafx.h"
#include "AICommon.h"
#include "AIEvaluatorComponent.h"
#include "AIEvaluatorUtil.h"
#include "AIHeuristics.h"
#include "BoardState.h"
#include "Tetris.h"

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

DesiredMoveSet AIEvaluatorUtil::__FindBestMove(BoardState* board, AIEvaluatorComponent * ownerComp, int numLookaheads, bool holdPiece)
{
	DesiredMoveSet result;

	numLookaheads--;

	if (!board->m_currentPiece)
	{
		return result;
	}

	for (int i = 0; i < board->m_cols; i++)
	{
		// Try all rotations
		int rotations = board->m_currentPiece->MaxRotations();
		for (int j = 0; j < rotations; j++)
		{
			// Make a copy of the board - just the game state, none of the presentation
			BoardState boardCopy;
			boardCopy.Clone(board);

			if (holdPiece)
			{
				boardCopy.SwapPiece();
				result.swapPiece = true;
			}

			for (int numRotations = 0; numRotations < j; numRotations++)
			{
				boardCopy.m_currentPiece->Rotate(true);
			}
			boardCopy.m_currentPiece->Move(-board->m_cols, 0);
			boardCopy.m_currentPiece->Move(i, 0);
			int colIdx = (int)boardCopy.m_currentPiece->m_originColIdx;
			boardCopy.DropCurrentPiece();

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
