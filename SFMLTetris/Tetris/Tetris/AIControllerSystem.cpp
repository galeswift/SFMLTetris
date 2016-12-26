#include "stdafx.h"
#include "AIControllerComponent.h"
#include "AIControllerSystem.h"
#include "Tetris.h"

AIControllerSystem::AIControllerSystem()
{
}

void AIControllerSystem::Update(float dt)
{
	for (s32 i = 0; i < g_clientGame.m_games.size(); i++)
	{
		Tetris* current = g_clientGame.m_games[i]->m_game;
		AIControllerComponent* comp = current->GetComponent<AIControllerComponent>();
		if (comp)
		{
			comp->m_timeUntilUpdate -= dt;

			if (comp->m_timeUntilUpdate <= 0.0f)
			{
				comp->m_timeUntilUpdate = comp->m_updateFrequency;
				if (!comp->m_currentMove.used && comp->m_owner->m_currentPiece)
				{
					if (comp->m_currentMove.swapPiece)
					{
						comp->m_currentMove.used = true;
						comp->m_owner->KeySwap();
					}
					else
					{
						bool shouldDrop = true;
						if (comp->m_owner->m_currentPiece->m_originRotations < comp->m_currentMove.numRotations)
						{
							shouldDrop = false;
							comp->m_owner->KeyRotate();
						}
						else if (comp->m_owner->m_currentPiece->m_originColIdx < comp->m_currentMove.col)
						{
							shouldDrop = false;
							comp->m_owner->KeyMoveRight();
						}
						else if (comp->m_owner->m_currentPiece->m_originColIdx > comp->m_currentMove.col)
						{
							shouldDrop = false;
							comp->m_owner->KeyMoveLeft();
						}

						if (shouldDrop)
						{
							comp->m_currentMove.used = true;
							comp->m_owner->KeyDrop();
						}
					}
				}
			}
		}
	}
}
