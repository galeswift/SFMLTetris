// Tetris.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

// Other includes
#include <SFML/Graphics.hpp>
#include "CombatUtil.h"
#include "SoundComponent.h"
#include "Tetris.h"

void Block::Draw(sf::RenderWindow * window, const sf::Vector2f& vecOrigin, sf::RectangleShape& blockShape, int colOffset, int rowOffset, bool ghost)
{
	sf::Vector2f position = vecOrigin;
	position += sf::Vector2f((float)(colOffset + colIdx) * GRID_SIZE, (float)(rowOffset + rowIdx)*GRID_SIZE);
	blockShape.setFillColor(m_fillColor);
	blockShape.setOutlineColor(m_outlineColor);
	blockShape.setPosition(position);

	if (ghost)
	{		
		sf::Color ghostFillColor = blockShape.getFillColor();
		sf::Color ghostOutlineColor = blockShape.getOutlineColor();
		ghostFillColor.a = 0;
		
		blockShape.setFillColor(ghostFillColor);
		blockShape.setOutlineColor(ghostOutlineColor);
		
		window->draw(blockShape);

		blockShape.setFillColor(m_fillColor);
		blockShape.setOutlineColor(m_outlineColor);
	}
	else
	{
		window->draw(blockShape);
	}
}

void Piece::Init(Tetris* board)
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

		Block& block = m_board->m_grid.m_cells[(int)(m_originColIdx + m_blocks[i].colIdx)][(int)(m_originRowIdx + m_blocks[i].rowIdx)].m_block;
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

void Piece::Draw(sf::RenderWindow* window, bool ghost)
{
	sf::Vector2f vecOrigin = m_board ? m_board->GetFieldShape()->getPosition() : sf::Vector2f(0, 0);
	for (int i=0 ; i<4 ; i++)
	{
		m_blocks[i].Draw(window, vecOrigin, m_board->m_blockShape, m_originColIdx , m_originRowIdx, ghost);
	}
}

Tetris::Tetris()
	: m_currentPiece(NULL)
	, m_heldPiece(NULL)
	, m_canSwapPiece(true)
	, m_isRunning(true)
	, m_clearedRows(0)
	, m_currentLevel(1)
	, m_levelDropSpeed(0.0f)	
	, m_isClone(false)
	, m_pieceID(0)
	, m_resetCount(0)	
	, m_boardIsPlayable(true)
{
	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		m_previewPieces[i] = NULL;
	}	
}

Tetris::~Tetris()
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

void Tetris::Clone(const Tetris* other)
{
	m_isClone = true;
	
	m_currentPiece = other->m_currentPiece ? new Piece(*other->m_currentPiece) : NULL;
	if (m_currentPiece) m_currentPiece->Init(this);

	m_heldPiece = other->m_heldPiece ? new Piece(*other->m_heldPiece) : NULL;
	if (m_heldPiece) m_heldPiece->Init(this);

	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		m_previewPieces[i] = new Piece(*other->m_previewPieces[i]);		
		if (m_previewPieces[i]) m_previewPieces[i]->Init(this);
	}

	m_grid = other->m_grid;	
	m_clearedRows = other->m_clearedRows;
	m_pieceID = other->m_pieceID;
	m_rows = other->m_rows;
	m_cols = other->m_cols;
	m_resetCount = other->m_resetCount;
}

Piece* Tetris::CreatePiece(PieceType type)
{	
	//type = SHAPE_TETRIS;

	// TODO: Switch based on piece type
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
	case SHAPE_CUBE: // J piece
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
	case SHAPE_TETRIS: // J piece
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

void Tetris::FillPieceShape(Block* block, sf::Color& pieceColor, sf::Color& outlineColor)
{	
	if (block)
	{
		block->m_fillColor = pieceColor;
		block->m_outlineColor = outlineColor;		
	}
}

void Tetris::Init(bool isPlayer, int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;
	
	sf::Vector2f fieldOrigin(FIELD_ORIGIN_X,FIELD_ORIGIN_Y);	
	for (int i=0 ; i < m_cols ; i++)
	{
		for (int j=0 ; j < m_rows ; j++)
		{			
			Block& block = m_grid.m_cells[i][j].m_block;
			block.colIdx = i;
			block.rowIdx = j;
		
			m_grid.m_cells[i][j].m_isFilled = false;
		}
	}
	
	InitResources();
	if (isPlayer)
	{
		InitKeyBindings();
	}
	Reset();
}

void Tetris::InitResources()
{
	m_mainFont.loadFromFile("Content/Fonts/MainFont.ttf");
	m_debugFont.loadFromFile("Content/Fonts/Debug.TTF");	
}

void Tetris::Reset()
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
	m_repeatTimer = INPUT_REPEAT_INTERVAL;	
	m_currentLevel = 1;
	m_clearedRows = 0;

	int nCurrentXPos = FIELD_ORIGIN_X;
	int nCurrentYPos = FIELD_ORIGIN_Y;

	m_fieldWidth = (GRID_SIZE * m_cols);
	m_fieldHeight = (GRID_SIZE * m_rows);
	m_fieldOrigin = sf::Vector2f(nCurrentXPos, nCurrentYPos);

	m_swapContainerShape.setSize(sf::Vector2f(SWAP_PANEL_SIZE, SWAP_PANEL_SIZE));
	m_swapContainerShape.setPosition(sf::Vector2f(nCurrentXPos, nCurrentYPos));
	m_swapContainerShape.setFillColor(GRID_FILL_COLOR);
	m_swapContainerShape.setOutlineColor(FIELD_OUTLINE_COLOR);
	m_swapContainerShape.setOutlineThickness(FIELD_OUTLINE_THICKNESS);
	nCurrentXPos += SWAP_PANEL_SIZE + GRID_SIZE * 2;

	m_fieldShape.setSize(sf::Vector2f(m_fieldWidth, m_fieldHeight));
	m_fieldShape.setPosition(sf::Vector2f(nCurrentXPos, nCurrentYPos));
	m_fieldShape.setFillColor(sf::Color::Blue);
	m_fieldShape.setOutlineColor(FIELD_OUTLINE_COLOR);
	m_fieldShape.setOutlineThickness(FIELD_OUTLINE_THICKNESS);
	nCurrentXPos += m_fieldWidth + GRID_SIZE * 2;

	m_previewContainerShape.setSize(sf::Vector2f(PREVIEW_PANEL_SIZE, PREVIEW_PANEL_SIZE * NUM_PREVIEW_PIECES));
	m_previewContainerShape.setPosition(sf::Vector2f(nCurrentXPos, nCurrentYPos));
	m_previewContainerShape.setFillColor(GRID_FILL_COLOR);
	m_previewContainerShape.setOutlineColor(FIELD_OUTLINE_COLOR);
	m_previewContainerShape.setOutlineThickness(FIELD_OUTLINE_THICKNESS);
	nCurrentXPos += PREVIEW_PANEL_SIZE;

	m_totalGameWidth = nCurrentXPos - FIELD_ORIGIN_X;
	m_totalGameHeight = m_fieldHeight - FIELD_ORIGIN_Y;

	m_blockShape.setSize(sf::Vector2f(GRID_SIZE, GRID_SIZE));
	m_blockShape.setOutlineThickness(BLOCK_OUTLINE_THICKNESS);
	m_blockShape.setOrigin(sf::Vector2f(0.5f, 0.5f));
}

void Tetris::OnBoardFilled()
{
	m_boardIsPlayable = false;	
}

void Tetris::CreateNewPiece(bool deleteCurrent)
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

void Tetris::InitKeyBindings()
{
	// Input bindings
	/*{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Escape;
		mapping.InputFunc = &Tetris::KeyExit;
		m_inputs.push_back(mapping);
	}*/

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::LAlt;
		mapping.InputFunc = &Tetris::KeyGarbage;
		m_inputs.push_back(mapping);
	}


	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::RAlt;
		mapping.InputFunc = &Tetris::KeyClearRow;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Up;
		mapping.InputFunc = &Tetris::KeyRotate;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Right;
		mapping.InputFunc = &Tetris::KeyMoveRight;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Left;
		mapping.InputFunc = &Tetris::KeyMoveLeft;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Down;
		mapping.InputFunc = &Tetris::KeyMoveDown;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Space;
		mapping.InputFunc = &Tetris::KeyDrop;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::LControl;
		mapping.InputFunc = &Tetris::KeySwap;
		m_inputs.push_back(mapping);
	}
}

void Tetris::KeyExit()
{
	m_isRunning = false;
}

void Tetris::KeyGarbage()
{
	CombatUtil::Attack(this, 4);
}

void Tetris::KeyClearRow()
{
	ClearRow(m_rows-1);
}

void Tetris::KeyRotate()
{
	if (m_currentPiece)
	{
		if (m_currentPiece->Rotate(true))
		{
			if (GetComponent<SoundComponent>() != nullptr)
			{
				GetComponent<SoundComponent>()->m_queuedSounds.push_back("Content/Sounds/block_rotate.wav");
			}

			m_currentPiece->m_originRotations++;
			if (m_currentPiece->m_originRotations > 3)
			{
				m_currentPiece->m_originRotations = 0;
			}
		}
	}
}

void Tetris::KeyMoveLeft()
{
	if (m_currentPiece)
	{
		m_currentPiece->Move(-1, 0);
	}
}

void Tetris::KeyMoveRight()
{
	if (m_currentPiece)
	{
		m_currentPiece->Move(1,0);
	}
}

void Tetris::KeyMoveDown()
{
	if (m_currentPiece)
	{
		if (!m_currentPiece->Move(0, 1))
		{
			DropCurrentPiece();
		}
	}
}

void Tetris::KeyDrop()
{
	if (m_currentPiece)
	{
		if (GetComponent<SoundComponent>() != nullptr)
		{
			GetComponent<SoundComponent>()->m_queuedSounds.push_back("Content/Sounds/block_drop.wav");
		}
		DropCurrentPiece();
	}
}

void Tetris::KeySwap()
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

int Tetris::GetTotalGameHeight()
{
	return m_totalGameHeight;
}

int Tetris::GetTotalGameWidth()
{
	return m_totalGameWidth;
}

const sf::RectangleShape* Tetris::GetFieldShape()
{
	return &m_fieldShape;
}

void Tetris::Update(float dt)
{	
	for (int i = 0; i < m_inputs.size(); i++)
	{
		InputMapping& mapping = m_inputs[i];
		if (sf::Keyboard::isKeyPressed(mapping.key))
		{			
			if (!mapping.m_isHeld || mapping.m_timeHeld == 0.0f)
			{
				(this->*(mapping.InputFunc))();

				// First time pressing, introduce a delay
				if (!mapping.m_isHeld)
				{
					mapping.m_timeHeld -= INPUT_REPEAT_START_DELAY;
				}

				mapping.m_isHeld = true;
			}			
			
			if (mapping.m_isHeld)
			{
				mapping.m_timeHeld += dt;

				if (mapping.m_timeHeld >= INPUT_REPEAT_INTERVAL)
				{
					mapping.m_timeHeld = 0.0f;					
				}
			}			
		}
		else
		{
			mapping.m_isHeld = false;
			mapping.m_timeHeld = 0.0f;
		}
	}
	
	if (!m_boardIsPlayable)
	{
		// Maybe reset?  Otherwise just sit here
		// Reset
	}
	else if (m_currentPiece != NULL)
	{
		float dropSpeed = GetDropSpeed();
		m_currentPiece->m_originRowIdx += dropSpeed *dt;
		if (!m_currentPiece->PositionIsValid())
		{
			m_currentPiece->m_originRowIdx -= dropSpeed * dt;
			DropCurrentPiece();
		}		
	}

}

void Tetris::Draw(sf::RenderWindow* window)
{		
	window->draw(m_fieldShape);	
	window->draw(m_previewContainerShape);
	window->draw(m_swapContainerShape);

	for (int i = 0; i < m_cols; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			m_blockShape.setOutlineColor(GRID_LINE_COLOR);
			m_blockShape.setFillColor(GRID_FILL_COLOR);			
			m_blockShape.setPosition(m_fieldShape.getPosition() + sf::Vector2f(i * GRID_SIZE, j * GRID_SIZE));
			window->draw(m_blockShape);

		}
	}

	for (int i = 0; i < m_cols; i++)
	{
		for (int j = 0; j < m_rows; j++)
		{
			if (m_grid.m_cells[i][j].m_isFilled)
			{
				Block& block = m_grid.m_cells[i][j].m_block;
				block.Draw(window, m_fieldShape.getPosition(), m_blockShape, 0, 0, false);
			}
		}
	}

	if (m_currentPiece != NULL)
	{
		m_currentPiece->Draw(window, false);

		// Draw a ghost piece
		float curCol = m_currentPiece->m_originColIdx;
		float curRow = m_currentPiece->m_originRowIdx;		
		
		m_currentPiece->Move(0, m_rows);
		m_currentPiece->Draw(window, true);

		m_currentPiece->m_originColIdx = curCol;
		m_currentPiece->m_originRowIdx = curRow;
	}

	if (m_heldPiece != NULL)
	{
		m_heldPiece->Draw(window, false);
	}

	// Draw preview
	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{		
		if (m_previewPieces[i] != NULL)
		{
			m_previewPieces[i]->Draw(window, false);
		}
	}
	// Draw 'HUD'
	sf::Text text;
	text.setPosition(m_fieldShape.getPosition() + sf::Vector2f(345, 475));
	text.setFont(m_mainFont);
	text.setCharacterSize(18);
	text.setFillColor(sf::Color(5,5,5,212));
	
	// Draw current level
	{
		std::string textStr = "CURRENT LEVEL ";
		textStr += std::to_string(m_currentLevel);
		text.setString(textStr.c_str());
		window->draw(text);
	}

	text.setPosition(text.getPosition() + sf::Vector2f(0, 35));
	// Draw current level
	{
		std::string textStr = "CLEARED ROWS  ";
		textStr += std::to_string(m_clearedRows);
		text.setString(textStr.c_str());
		window->draw(text);
	}

	// Draw headers
	{
		std::string textStr = "HELD";
		text.setPosition(m_fieldOrigin + sf::Vector2f(0, -50));
		text.setString(textStr.c_str());
		window->draw(text);
	}

	{
		std::string textStr = "NEXT";
		text.setPosition(m_fieldOrigin + sf::Vector2f(m_totalGameWidth - PREVIEW_PANEL_SIZE, -50));
		text.setString(textStr.c_str());
		window->draw(text);
	}
}

void Tetris::DropCurrentPiece()
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

void Tetris::ClearRow(int row)
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

void Tetris::UpdateCurrentLevel()
{
	m_currentLevel = 1 + m_clearedRows/10;
	m_levelDropSpeed = 1;// (m_currentLevel - 1) * 0.5f;
}

void Tetris::DropRow(int row)
{
	if (row < m_rows)
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

void Tetris::AddGarbage(int numRows)
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

bool Tetris::IsRunning()
{
	return m_isRunning;
}

bool Tetris::IsBoardPlayable()
{
	return m_boardIsPlayable;
}

float Tetris::GetDropSpeed()
{
	float result = DROP_SPEED + m_levelDropSpeed;
	return (result < MAX_DROP_SPEED ? result : MAX_DROP_SPEED);
}
