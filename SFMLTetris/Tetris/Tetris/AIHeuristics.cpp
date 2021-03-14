#include "stdafx.h"
#include "AIHeuristics.h"
#include "Tetris.h"

float AIHeuristic_AggregateHeight::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;

	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			if (tetrisBoard->m_grid.m_cells[i][j].m_isFilled)
			{
				result += tetrisBoard->m_rows - j;
				break;
			}
		}
	}

	return m_scalar * result;
}

float AIHeuristic_CompletedLines::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = (float)(tetrisBoard->m_clearedRows - original->m_clearedRows);
	return m_scalar * result;
}

float AIHeuristic_Holes::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;

	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		bool bFoundBlock = false;
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][j];
			if (gridCell.m_isFilled)
			{
				bFoundBlock = true;
			}
			else if (bFoundBlock)
			{
				result++;
			}
		}
	}

	return m_scalar * result;
}

float AIHeuristic_Bumpiness::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;

	int prevHeight = 0;
	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		int height = 0;
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][j];
			if (gridCell.m_isFilled)
			{
				height = tetrisBoard->m_rows - j;
				break;
			}
		}

		if (i > 0)
		{
			result += abs(prevHeight - height);
		}

		prevHeight = height;
	}


	return m_scalar * result;
}

float AIHeuristic_HighestCol::GetScore(const Tetris * original, Tetris * tetrisBoard)
{
	float result = 0.0f;
	int highest = 0;
	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			if (tetrisBoard->m_grid.m_cells[i][j].m_isFilled)
			{
				int height = tetrisBoard->m_rows - j;
				if (highest < height)
				{
					highest = height;
					if (height + 6 > tetrisBoard->m_rows)
					{
						result = (float)height + 6;
					}
					else
					{
						result = (float)height;
					}
				}

				break;
			}
		}
	}

	return m_scalar * result;
}

float AIHeuristic_GameLoss::GetScore(const Tetris * original, Tetris * tetrisBoard)
{
	float result = 0.0f;
	if (tetrisBoard->m_resetCount > original->m_resetCount)
	{
		result = -100000000;
	}
	return result;
}

float AIHeuristic_Blockade::GetScore(const Tetris * original, Tetris * tetrisBoard)
{
	float result = 0.0f;

	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		int blockadeCount = 0;
		bool bFoundBlock = false;
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][j];
			if (gridCell.m_isFilled)
			{
				blockadeCount++;
				bFoundBlock = true;
			}
			else if (bFoundBlock)
			{
				result += blockadeCount;
				break;
			}
		}
	}

	return m_scalar * result;
}

float AIHeuristic_DeepestHole::GetScore(const Tetris* original, Tetris* tetrisBoard)
{
	float result = 0.0f;

	int leftCol = -1;
	int rightCol = tetrisBoard->m_cols;
	int holeDepth = 0;
	int deepestHole = 0;
	for (int i = 0; i < tetrisBoard->m_cols; i++)
	{
		bool bFoundBlock = false;
		for (int j = 0; j < tetrisBoard->m_rows; j++)
		{
			GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][j];
			if (!gridCell.m_isFilled)
			{
				leftCol = i - 1;
				rightCol = i + 1;
				holeDepth=0;
				if (leftCol >= -1 && rightCol <= tetrisBoard->m_cols)
				{					
					for (int k = j; k < tetrisBoard->m_rows; k++)
					{
						bool leftFilled = (leftCol >= 0) ? tetrisBoard->m_grid.m_cells[leftCol][k].m_isFilled : true;
						bool rightFilled = (rightCol < tetrisBoard->m_cols) ? tetrisBoard->m_grid.m_cells[rightCol][k].m_isFilled : true;
						GridCell& gridCell = tetrisBoard->m_grid.m_cells[i][k];
						if (leftFilled && rightFilled && !gridCell.m_isFilled)
						{
							holeDepth++;
						}
					}
				}
			}

			if (holeDepth > deepestHole)
			{
				deepestHole = holeDepth;
			}
		}
	}

	return m_scalar * deepestHole;
}
