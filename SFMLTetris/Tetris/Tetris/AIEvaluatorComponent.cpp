#include "stdafx.h"
#include "AIEvaluatorComponent.h"
#include "AIHeuristics.h"

AIEvaluatorComponent::AIEvaluatorComponent(Tetris* owner)
	: Component(owner)	
{
	m_timeSinceLastUpdate = 0.0f + rand() / (float)RAND_MAX;
}


AIEvaluatorComponent::~AIEvaluatorComponent()
{
}
