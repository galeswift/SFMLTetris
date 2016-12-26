#include "stdafx.h"
#include "AIControllerComponent.h"

AIControllerComponent::AIControllerComponent(Tetris* owner)
	: Component(owner)
	, m_timeUntilUpdate(AI_CONTROLLER_UPDATE_FREQUENCY)
	, m_updateFrequency(AI_CONTROLLER_UPDATE_FREQUENCY)
{
	m_currentMove.used = true;
}


AIControllerComponent::~AIControllerComponent()
{
}
