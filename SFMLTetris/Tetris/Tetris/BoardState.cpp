#include "stdafx.h"

#include "BoardState.h"

void Piece::Init(BoardState* board)
{
	m_board = board;
}

bool Piece::Rotate(bool right)
{
	if (m_type == SHAPE_CUBE)
	{
		return true;
	}

	if (__TryRotate(right))
	{
		return true;
	}

	// One space left
	if (Move(-1, 0))
	{
		if (__TryRotate(right))
		{
			return true;
		}
		else if (Move(2, 0))
		{
			if (__TryRotate(right))
			{
				return true;
			}
		}
	}
	else if (Move(1,0))
	{
		if (__TryRotate(right))
		{
			return true;
		}
		else
		{
			Move(-1, 0);
		}
	}

	return false;
}

bool Piece::Move(int x, int y)
{
	int xMove = x > 0 ? 1 : -1;
	for (int i = 0; i < abs(x); i++)
	{
		m_originColIdx += xMove;

		// See if this move is ok?
		if (!PositionIsValid())
		{
			m_originColIdx-=xMove;
			return false;
		}
	}
	int yMove = y > 0 ? 1 : -1;
	for (int i = 0; i < abs(y); i++)
	{
		m_originRowIdx += yMove;

		// See if this move is ok?
		if (!PositionIsValid())
		{
			m_originRowIdx -= yMove;
			return false;
		}
	}

	return true;
}

bool Piece::PositionIsValid()
{
	// Check each piece against the board
	for (int i = 0; i < 4; i++)
	{
		int col = (int)(m_blocks[i].colIdx + m_originColIdx);
		int row = (int)(m_blocks[i].rowIdx + m_originRowIdx);

		if (col < 0)
		{
			return false;
		}
		if (col >= m_board->m_cols)
		{
			return false;
		}

		if (row >= m_board->m_rows)
		{
			return false;
		}

		GridCell& cell = m_board->m_grid.m_cells[col][row];
		if (cell.m_isFilled)
		{
			return false;
		}
	}

	return true;
}

void Piece::Drop()
{
	// Drop
	Move(0, m_board->m_rows);

	// place the pieces on the grid
	for (int i = 0; i < 4; i++)
	{
		GridCell& cell = m_board->m_grid.m_cells[(int)(m_originColIdx + m_blocks[i].colIdx)][(int)(m_originRowIdx + m_blocks[i].rowIdx)];
		cell.m_isFilled = true;

		Block& block = cell.m_block;
		block.m_fillColor = m_blocks[i].m_fillColor;
		block.m_outlineColor = m_blocks[i].m_outlineColor;
	}
}

void Piece::PlaceIntoHolding()
{
	m_originColIdx = -5;
	m_originRowIdx = 1;
}

void Piece::PlaceIntoGrid()
{
	m_originColIdx = (int)(m_board->m_cols * 0.5f) - 1;
	m_originRowIdx = 0;

	if (!PositionIsValid())
	{
		m_board->OnBoardFilled();
	}
}

void Piece::PlaceIntoPreview(int slot)
{
	m_originColIdx = m_board->m_cols + 3;
	m_originRowIdx = 1 + slot * 5;
}

bool Piece::__TryRotate(bool right)
{
	bool result = true;
	__DoRotate(right);
	// Try moving this around
	if (!PositionIsValid())
	{
		__DoRotate(!right);
		result = false;
	}

	return result;
}

void Piece::__DoRotate(bool right)
{
	if (right)
	{
		for (int i = 0; i < 4; i++)
		{
			int oldCol = m_blocks[i].colIdx;
			m_blocks[i].colIdx = m_pivotCol + m_pivotRow - m_blocks[i].rowIdx;
			m_blocks[i].rowIdx = m_pivotRow - m_pivotCol + oldCol;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			int oldrow = m_blocks[i].rowIdx;
			m_blocks[i].rowIdx = m_pivotRow + m_pivotCol - m_blocks[i].colIdx;
			m_blocks[i].colIdx = m_pivotCol - m_pivotRow + oldrow;
		}
	}

}

Piece::Piece(const Piece& other)
{
	m_board = other.m_board;
	m_originRowIdx = other.m_originRowIdx;
	m_originColIdx = other.m_originColIdx;
	m_originRotations = other.m_originRotations;
	m_pivotRow = other.m_pivotRow;
	m_pivotCol = other.m_pivotCol;
	m_maxRotations = other.m_maxRotations;

	m_type = other.m_type;

	for (int i = 0; i < 4; i++)
	{
		this->m_blocks[i] = other.m_blocks[i];
	}
}

BoardState::BoardState()
	: m_currentPiece(NULL)
	, m_heldPiece(NULL)
	, m_canSwapPiece(true)
	, m_boardIsPlayable(true)
	, m_isClone(false)
	, m_clearedRows(0)
	, m_currentLevel(1)
	, m_levelDropSpeed(0.0f)
	, m_pieceID(0)
	, m_rows(NUM_ROWS)
	, m_cols(NUM_COLS)
	, m_resetCount(0)
	, m_lastClearCount(0)
{
	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		m_previewPieces[i] = NULL;
	}
}

BoardState::~BoardState()
{
	delete m_currentPiece;
	m_currentPiece = NULL;

	delete m_heldPiece;
	m_heldPiece = NULL;

	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		delete m_previewPieces[i];
		m_previewPieces[i] = NULL;
	}
}

void BoardState::InitBoard(int rows, int cols)
{
	// The grid storage is compile-time sized; clamp requested dimensions to fit
	if (rows > NUM_ROWS) rows = NUM_ROWS;
	if (cols > NUM_COLS) cols = NUM_COLS;
	if (rows < 4) rows = 4;
	if (cols < 4) cols = 4;

	m_rows = rows;
	m_cols = cols;

	for (int i = 0; i < m_cols; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			Block& block = m_grid.m_cells[i][j].m_block;
			block.colIdx = i;
			block.rowIdx = j;

			m_grid.m_cells[i][j].m_isFilled = false;
		}
	}
}

void BoardState::ResetBoard()
{
	if (m_heldPiece != NULL)
	{
		delete m_heldPiece;
		m_heldPiece = NULL;
	}

	if (m_currentPiece != NULL)
	{
		delete m_currentPiece;
		m_currentPiece = NULL;
	}

	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		if (m_previewPieces[i] != NULL)
		{
			delete m_previewPieces[i];
			m_previewPieces[i] = NULL;
		}
	}
	for (int i = 0; i < m_cols; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			m_grid.m_cells[i][j].m_isFilled = false;
		}
	}

	CreateNewPiece(true);
	m_resetCount++;
	m_currentLevel = 1;
	m_clearedRows = 0;
}

void BoardState::Clone(const BoardState* other)
{
	m_isClone = true;

	m_currentPiece = other->m_currentPiece ? new Piece(*other->m_currentPiece) : NULL;
	if (m_currentPiece) m_currentPiece->Init(this);

	m_heldPiece = other->m_heldPiece ? new Piece(*other->m_heldPiece) : NULL;
	if (m_heldPiece) m_heldPiece->Init(this);

	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		m_previewPieces[i] = other->m_previewPieces[i] ? new Piece(*other->m_previewPieces[i]) : NULL;
		if (m_previewPieces[i]) m_previewPieces[i]->Init(this);
	}

	m_grid = other->m_grid;
	m_clearedRows = other->m_clearedRows;
	m_pieceID = other->m_pieceID;
	m_rows = other->m_rows;
	m_cols = other->m_cols;
	m_resetCount = other->m_resetCount;
}

Piece* BoardState::CreatePiece(PieceType type)
{
	Piece* result = new Piece();
	result->Init(this);

	result->m_type = type;
	result->m_maxRotations = 3;
	result->m_id = m_pieceID++;

	sf::Color pieceColor;
	switch(type)
	{
	case SHAPE_T: // T piece
		result->m_blocks[0].rowIdx = 1;
		result->m_blocks[0].colIdx = 0;

		result->m_blocks[1].rowIdx = 0;
		result->m_blocks[1].colIdx = 1;

		result->m_blocks[2].rowIdx = 1;
		result->m_blocks[2].colIdx = 1;

		result->m_blocks[3].rowIdx = 2;
		result->m_blocks[3].colIdx = 1;

		result->m_pivotCol = 1;
		result->m_pivotRow = 1;

		result->m_maxRotations = 3;
		pieceColor = sf::Color(138, 43, 226);
		break;
	case SHAPE_Z: // Z piece
		result->m_blocks[0].rowIdx = 0;
		result->m_blocks[0].colIdx = 0;

		result->m_blocks[1].rowIdx = 0;
		result->m_blocks[1].colIdx = 1;

		result->m_blocks[2].rowIdx = 1;
		result->m_blocks[2].colIdx = 1;

		result->m_blocks[3].rowIdx = 1;
		result->m_blocks[3].colIdx = 2;

		result->m_pivotCol = 1;
		result->m_pivotRow = 1;
		result->m_maxRotations = 2;
		pieceColor = sf::Color(255, 0, 0);
		break;
	case SHAPE_S: // S piece
		result->m_blocks[0].rowIdx = 1;
		result->m_blocks[0].colIdx = 0;

		result->m_blocks[1].rowIdx = 0;
		result->m_blocks[1].colIdx = 1;

		result->m_blocks[2].rowIdx = 1;
		result->m_blocks[2].colIdx = 1;

		result->m_blocks[3].rowIdx = 0;
		result->m_blocks[3].colIdx = 2;

		result->m_pivotCol = 1;
		result->m_pivotRow = 1;

		result->m_maxRotations = 2;
		pieceColor = sf::Color(0, 255, 0);
		break;
	case SHAPE_L: // L piece
		result->m_blocks[0].rowIdx = 0;
		result->m_blocks[0].colIdx = 0;

		result->m_blocks[1].rowIdx = 1;
		result->m_blocks[1].colIdx = 0;

		result->m_blocks[2].rowIdx = 2;
		result->m_blocks[2].colIdx = 0;

		result->m_blocks[3].rowIdx = 2;
		result->m_blocks[3].colIdx = 1;

		result->m_pivotCol = 0;
		result->m_pivotRow = 1;

		result->m_maxRotations = 3;
		pieceColor = sf::Color(255, 165, 0);
		break;
	case SHAPE_J: // J piece
		result->m_blocks[0].rowIdx = 0;
		result->m_blocks[0].colIdx = 1;

		result->m_blocks[1].rowIdx = 1;
		result->m_blocks[1].colIdx = 1;

		result->m_blocks[2].rowIdx = 2;
		result->m_blocks[2].colIdx = 1;

		result->m_blocks[3].rowIdx = 2;
		result->m_blocks[3].colIdx = 0;

		result->m_pivotCol = 1;
		result->m_pivotRow = 1;

		result->m_maxRotations = 3;
		pieceColor = sf::Color(0, 0, 255);
		break;
	case SHAPE_CUBE: // Cube piece
		result->m_blocks[0].rowIdx = 0;
		result->m_blocks[0].colIdx = 0;

		result->m_blocks[1].rowIdx = 0;
		result->m_blocks[1].colIdx = 1;

		result->m_blocks[2].rowIdx = 1;
		result->m_blocks[2].colIdx = 0;

		result->m_blocks[3].rowIdx = 1;
		result->m_blocks[3].colIdx = 1;

		result->m_pivotCol = 1;
		result->m_pivotRow = 1;

		result->m_maxRotations = 1;
		pieceColor = sf::Color(255, 255, 0);
		break;
	case SHAPE_TETRIS: // I piece
		result->m_blocks[0].rowIdx = 0;
		result->m_blocks[0].colIdx = 0;

		result->m_blocks[1].rowIdx = 1;
		result->m_blocks[1].colIdx = 0;

		result->m_blocks[2].rowIdx = 2;
		result->m_blocks[2].colIdx = 0;

		result->m_blocks[3].rowIdx = 3;
		result->m_blocks[3].colIdx = 0;
		result->m_pivotCol = 0;
		result->m_pivotRow = 2;

		result->m_maxRotations = 2;
		pieceColor = sf::Color(0, 255, 255);
		break;
	}

	sf::Color outlineColor = pieceColor;
	outlineColor.r *= 0.5f;
	outlineColor.g *= 0.5f;
	outlineColor.b *= 0.5f;
	for (int i = 0; i < 4; i++)
	{
		FillPieceShape(&result->m_blocks[i], pieceColor, outlineColor);
	}
	result->m_originRotations = 0;
	return result;
}

void BoardState::FillPieceShape(Block* block, const sf::Color& pieceColor, const sf::Color& outlineColor)
{
	if (block)
	{
		block->m_fillColor = pieceColor;
		block->m_outlineColor = outlineColor;
	}
}

void BoardState::OnBoardFilled()
{
	m_boardIsPlayable = false;
}

void BoardState::CreateNewPiece(bool deleteCurrent)
{
	m_canSwapPiece = true;

	if (deleteCurrent && m_currentPiece != NULL)
	{
		delete m_currentPiece;
		m_currentPiece = NULL;
	}

	if (NUM_PREVIEW_PIECES > 0)
	{
		// Test to see if the preview pieces should be initialized
		if (m_previewPieces[0] == NULL)
		{
			m_currentPiece = CreatePiece((PieceType)(rand() % SHAPE_COUNT));
			m_currentPiece->PlaceIntoGrid();

			for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
			{
				m_previewPieces[i] = CreatePiece((PieceType)(rand() % SHAPE_COUNT));
				m_previewPieces[i]->PlaceIntoPreview(i);
			}
		}
		// Move a preview into the game
		else
		{
			m_currentPiece = m_previewPieces[0];
			if (NUM_PREVIEW_PIECES > 1)
			{
				for (int i = 0; i < NUM_PREVIEW_PIECES - 1; i++)
				{
					m_previewPieces[i] = m_previewPieces[i + 1];
					m_previewPieces[i]->PlaceIntoPreview(i);
				}
			}

			m_previewPieces[NUM_PREVIEW_PIECES - 1] = CreatePiece((PieceType)(rand() % SHAPE_COUNT));
			m_previewPieces[NUM_PREVIEW_PIECES - 1]->PlaceIntoPreview(NUM_PREVIEW_PIECES - 1);

			// Place after adjusting preview since placing it might reset the board
			m_currentPiece->PlaceIntoGrid();
		}
	}
	else
	{
		// Just place a piece
		m_currentPiece = CreatePiece((PieceType)(rand() % SHAPE_COUNT));
		m_currentPiece->PlaceIntoGrid();
	}
}

bool BoardState::RotateCurrentPiece()
{
	if (m_currentPiece && m_currentPiece->Rotate(true))
	{
		m_currentPiece->m_originRotations++;
		if (m_currentPiece->m_originRotations > 3)
		{
			m_currentPiece->m_originRotations = 0;
		}
		return true;
	}
	return false;
}

void BoardState::SwapPiece()
{
	if (m_currentPiece && m_canSwapPiece)
	{
		if (m_heldPiece == NULL)
		{
			m_heldPiece = m_currentPiece;
			m_heldPiece->PlaceIntoHolding();
			CreateNewPiece(false);
		}
		else
		{
			Piece* oldPiece = m_currentPiece;
			m_currentPiece = m_heldPiece;
			m_heldPiece = oldPiece;
			m_heldPiece->PlaceIntoHolding();
			m_currentPiece->PlaceIntoGrid();
		}

		// Can only swap once per new piece
		m_canSwapPiece = false;
	}
}

void BoardState::DropCurrentPiece()
{
	m_currentPiece->Drop();
	CreateNewPiece(true);

	// Check for clears
	int numClears = 0;
	for (int i = 0; i < m_rows; i++)
	{
		bool clear = true;
		for (int j = 0; j < m_cols; j++)
		{
			if (!m_grid.m_cells[j][i].m_isFilled)
			{
				clear = false;
				break;
			}
		}

		if (clear)
		{
			numClears++;
			ClearRow(i);
		}
	}

	m_lastClearCount = numClears;
}

void BoardState::ClearRow(int row)
{
	// Clear this line
	for (int i = 0; i < m_cols; i++)
	{
		m_grid.m_cells[i][row].m_isFilled = false;
	}

	m_clearedRows++;

	UpdateCurrentLevel();

	// Move every row above it down
	for (int i = row - 1; i >= 0; i--)
	{
		DropRow(i);
	}
}

void BoardState::UpdateCurrentLevel()
{
	m_currentLevel = 1 + m_clearedRows/10;
	m_levelDropSpeed = 1;// (m_currentLevel - 1) * 0.5f;
}

void BoardState::DropRow(int row)
{
	if (row + 1 < m_rows)
	{
		for (int i = 0; i < m_cols; i++)
		{
			if (m_grid.m_cells[i][row].m_isFilled)
			{
				m_grid.m_cells[i][row + 1].m_block.m_fillColor = m_grid.m_cells[i][row].m_block.m_fillColor;
				m_grid.m_cells[i][row + 1].m_block.m_outlineColor = m_grid.m_cells[i][row].m_block.m_outlineColor;
				m_grid.m_cells[i][row + 1].m_isFilled = true;
				m_grid.m_cells[i][row].m_isFilled = false;
			}
		}
	}
}

void BoardState::AddGarbage(int numRows)
{
	for (int i = 0; i < (m_rows - numRows); i++)
	{
		for (int j = 0; j < m_cols; j++)
		{
			m_grid.m_cells[j][i].m_isFilled = m_grid.m_cells[j][i + numRows].m_isFilled;
			m_grid.m_cells[j][i].m_block.m_fillColor = m_grid.m_cells[j][i + numRows].m_block.m_fillColor;
			m_grid.m_cells[j][i].m_block.m_outlineColor = m_grid.m_cells[j][i + numRows].m_block.m_outlineColor;
		}
	}

	for (int i = (m_rows - numRows); i < m_rows; i++)
	{
		int holeIdx = rand() % m_cols;
		for (int j = 0; j < m_cols; j++)
		{
			// Pick a hole
			if (j != holeIdx)
			{
				m_grid.m_cells[j][i].m_isFilled = true;
				FillPieceShape(&m_grid.m_cells[j][i].m_block, sf::Color(255, 255, 255, 255), sf::Color(128, 128, 128, 255));
			}
			else
			{
				m_grid.m_cells[j][i].m_isFilled = false;
			}
		}
	}
}

bool BoardState::IsBoardPlayable()
{
	return m_boardIsPlayable;
}

float BoardState::GetDropSpeed()
{
	float result = DROP_SPEED + m_levelDropSpeed;
	return (result < MAX_DROP_SPEED ? result : MAX_DROP_SPEED);
}
