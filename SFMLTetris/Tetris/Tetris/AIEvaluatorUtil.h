#pragma once

#include "AICommon.h"

class AIEvaluatorComponent;
class BoardState;

class AIEvaluatorUtil
{
public:
	static void FindBestMove(AIEvaluatorComponent* comp);
private:
	static DesiredMoveSet __FindBestMove(BoardState* board, AIEvaluatorComponent* ownerComp, int lookaheads, bool holdPiece);
};
