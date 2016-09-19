#include "stdafx.h"
#include "AIController.h"
#include "Tetris.h"

AIController::AIController(Tetris * tetrisBoard)
	: m_tetrisBoard(tetrisBoard)
	, m_timeUntilUpdate(AI_CONTROLLER_UPDATE_FREQUENCY)
{
	m_currentMove.used = true;
}

void AIController::Update(float dt)
{
	m_timeUntilUpdate -= dt;
	
	if (m_timeUntilUpdate <= 0.0f)
	{
		m_timeUntilUpdate = AI_CONTROLLER_UPDATE_FREQUENCY;
		if (!m_currentMove.used && m_tetrisBoard->m_currentPiece)
		{
			bool shouldDrop = true;
			if (m_tetrisBoard->m_currentPiece->m_originRotations < m_currentMove.numRotations)
			{
				shouldDrop = false;
				m_tetrisBoard->KeyRotate();
			}
			else if (m_tetrisBoard->m_currentPiece->m_originColIdx < m_currentMove.col)
			{
				shouldDrop = false;
				m_tetrisBoard->KeyMoveRight();
			}
			else if (m_tetrisBoard->m_currentPiece->m_originColIdx > m_currentMove.col)
			{
				shouldDrop = false;
				m_tetrisBoard->KeyMoveLeft();
			}
		
			if (shouldDrop)
			{				
				m_currentMove.used = true;				
				m_tetrisBoard->KeyDrop();
			}
		}
	}
}
