#include "stdafx.h"
#include "AIControllerComponent.h"
#include "ComponentPool.h"

DEFINE_COMPONENT(AIControllerComponent)

AIControllerComponent::AIControllerComponent()
{
	Reset();
}

void AIControllerComponent::Reset()
{
	m_timeUntilUpdate = AI_CONTROLLER_UPDATE_FREQUENCY;
	m_updateFrequency = AI_CONTROLLER_UPDATE_FREQUENCY;
	m_currentMove = DesiredMoveSet();
	m_currentMove.used = true;
}
