#pragma once

#include "stdafx.h"
#include "Tetris.h"

// Sets up a board with valid dimensions but no resources (fonts, pieces,
// key bindings), so tests can run headless.
inline void InitBareBoard(Tetris& board, int rows = NUM_ROWS, int cols = NUM_COLS)
{
	board.m_rows = rows;
	board.m_cols = cols;
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			board.m_grid.m_cells[i][j].m_isFilled = false;
		}
	}
}

inline void FillCell(Tetris& board, int col, int row)
{
	board.m_grid.m_cells[col][row].m_isFilled = true;
}

// Fills the bottom `height` cells of a column.
inline void FillColumn(Tetris& board, int col, int height)
{
	for (int j = board.m_rows - height; j < board.m_rows; j++)
	{
		FillCell(board, col, j);
	}
}
