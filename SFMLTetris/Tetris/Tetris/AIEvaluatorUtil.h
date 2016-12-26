#pragma once

#include "AICommon.h"

class AIEvaluatorComponent;

class AIEvaluatorUtil
{
public:
	AIEvaluatorUtil();
	~AIEvaluatorUtil();

	static void FindBestMove(AIEvaluatorComponent* comp);
private:
	static DesiredMoveSet __FindBestMove(Tetris* tetrisBoard, class AIEvaluatorComponent* ownerComp, int lookaheads, bool holdPiece);
};

