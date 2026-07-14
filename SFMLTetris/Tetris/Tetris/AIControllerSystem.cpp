#include "stdafx.h"
#include "AIControllerComponent.h"
#include "AIControllerSystem.h"
#include "ClientGame.h"
#include "Tetris.h"

void AIControllerSystem::Update(GameManager& manager, float dt)
{
	for (ComponentIterator itr = manager.GetComponents(COMPONENT_AI_CONTROLLER); itr.Get() != nullptr; itr++)
	{
		AIControllerComponent* comp = itr.Get<AIControllerComponent>();

		Tetris* game = comp->m_owner;
		if (!game->IsBoardPlayable())
		{
			continue;
		}

		comp->m_timeUntilUpdate -= dt;

		while (comp->m_timeUntilUpdate <= 0.0f)
		{
			comp->m_timeUntilUpdate += comp->m_updateFrequency;
			if (!comp->m_currentMove.used && game->m_currentPiece)
			{
				if (comp->m_currentMove.swapPiece)
				{
					comp->m_currentMove.used = true;
					game->ApplyInput(INPUT_SWAP);
				}
				else
				{
					bool shouldDrop = true;
					if (game->m_currentPiece->m_originRotations < comp->m_currentMove.numRotations)
					{
						shouldDrop = false;
						game->ApplyInput(INPUT_ROTATE);
					}
					else if (game->m_currentPiece->m_originColIdx < comp->m_currentMove.col)
					{
						shouldDrop = false;
						game->ApplyInput(INPUT_MOVE_RIGHT);
					}
					else if (game->m_currentPiece->m_originColIdx > comp->m_currentMove.col)
					{
						shouldDrop = false;
						game->ApplyInput(INPUT_MOVE_LEFT);
					}

					if (shouldDrop)
					{
						comp->m_currentMove.used = true;
						game->ApplyInput(INPUT_DROP);
					}
				}
			}
		}
	}
}
