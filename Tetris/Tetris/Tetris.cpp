// Tetris.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

//Raknet includes
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"

// Other includes
#include <SFML/Graphics.hpp>
#include "Tetris.h"

TetrisNetworkBase* g_networkPtr = NULL;

void Block::Draw(sf::RenderWindow * window, int colOffset, int rowOffset, bool ghost)
{
	sf::Vector2f fieldOrigin(FIELD_ORIGIN_X, FIELD_ORIGIN_Y);	
	sf::Vector2f position = fieldOrigin;
	position += sf::Vector2f((colOffset + colIdx) * GRID_SIZE, (rowOffset + rowIdx)*GRID_SIZE);
	shape.setPosition(position);

	if (ghost)
	{
		sf::Color originalFillColor = shape.getFillColor();
		sf::Color originalOutlineColor = shape.getOutlineColor();

		sf::Color ghostFillColor = shape.getFillColor();
		sf::Color ghostOutlineColor = shape.getOutlineColor();
		ghostFillColor.a = 100;
		ghostOutlineColor.a = 100;

		shape.setFillColor(ghostFillColor);
		shape.setOutlineColor(ghostFillColor);
		
		window->draw(shape);

		shape.setFillColor(originalFillColor);
		shape.setOutlineColor(originalOutlineColor);
	}
	else
	{
		window->draw(shape);
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
		if (col >= NUM_COLS)
		{
			return false;
		}
		
		if (row >= NUM_ROWS)
		{
			return false;
		}

		GridCell& cell = m_board->m_grid[col][row];
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
	Move(0, NUM_ROWS);
	
	// place the pieces on the grid
	for (int i = 0; i < 4; i++)
	{
		GridCell& cell = m_board->m_grid[(int)(m_originColIdx + m_blocks[i].colIdx)][(int)(m_originRowIdx + m_blocks[i].rowIdx)];		
		cell.m_block.shape = m_blocks[i].shape;
		cell.m_isFilled = true;
	}
}

void Piece::PlaceIntoHolding()
{
	m_originColIdx = -5;
	m_originRowIdx = 1;
}

void Piece::PlaceIntoGrid()
{
	m_originColIdx = (int)(NUM_COLS * 0.5f) - 1;
	m_originRowIdx = 0;

	if (!PositionIsValid())
	{
		m_board->Reset();
	}
}

void Piece::PlaceIntoPreview(int slot)
{
	m_originColIdx = NUM_COLS + 5;
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

void Piece::Draw(sf::RenderWindow* window, bool ghost)
{
	for (int i=0 ; i<4 ; i++)
	{
		m_blocks[i].Draw(window, m_originColIdx, m_originRowIdx, ghost);
	}
}

Tetris::Tetris() :
	m_currentPiece(NULL) ,
	m_heldPiece(NULL) ,
	m_canSwapPiece(true) ,
	m_isRunning(true)
{
	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		m_previewPieces[i] = NULL;
	}
}

Piece* Tetris::CreatePiece(PieceType type)
{
	// TODO: Switch based on piece type
	Piece* result = new Piece();
	result->Init(this);
	
	result->m_type = type;

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

		pieceColor = sf::Color(0, 255, 255);
		break;
	}
	
	sf::Color outlineColor = pieceColor;
	outlineColor.r *= 0.75f;
	outlineColor.g *= 0.75f;
	outlineColor.b *= 0.75f;
	for (int i = 0; i < 4; i++)
	{		
		FillPieceShape(result->m_blocks[i].shape, pieceColor, outlineColor);		
	}
	return result;
}

void Tetris::FillPieceShape(sf::RectangleShape& grid, sf::Color& pieceColor, sf::Color& outlineColor)
{	
	grid.setFillColor(pieceColor);
	grid.setOutlineColor(outlineColor);
	grid.setOutlineThickness(2.0f);
	grid.setSize(sf::Vector2f(GRID_SIZE - grid.getOutlineThickness(), GRID_SIZE - grid.getOutlineThickness()));
}

void Tetris::Init()
{
	srand(unsigned int(time(NULL)));
	sf::Vector2f fieldOrigin(FIELD_ORIGIN_X,FIELD_ORIGIN_Y);
	for (int i=0 ; i < NUM_COLS ; i++)
	{
		for (int j=0 ; j < NUM_ROWS ; j++)
		{
			sf::RectangleShape& grid = m_grid[i][j].m_backgroundShape;			
			grid.setOutlineColor(sf::Color(255,255,255,50));
			grid.setFillColor(sf::Color(0,0,0));
			grid.setOutlineThickness(1.0f);
			grid.setOrigin(sf::Vector2f(0.5f, 0.5f));
			grid.setSize(sf::Vector2f(GRID_SIZE, GRID_SIZE));			
			grid.setPosition(fieldOrigin + sf::Vector2f(i * GRID_SIZE, j*GRID_SIZE));	
			
			Block& block = m_grid[i][j].m_block;
			block.colIdx = i;
			block.rowIdx = j;

			m_grid[i][j].m_isFilled = false;
		}
	}
	
	InitKeyBindings();

	Reset();
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
	for (int i = 0; i < NUM_COLS; i++)
	{
		for (int j = 0; j < NUM_ROWS; j++)
		{
			m_grid[i][j].m_isFilled = false;
		}
	}	

	CreateNewPiece(true);
	m_repeatTimer = INPUT_REPEAT_INTERVAL;	
}

void Tetris::CreateNewPiece(bool deleteCurrent)
{
	m_canSwapPiece = true;

	if (deleteCurrent && m_currentPiece != NULL)
	{
		delete m_currentPiece;
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
			m_previewPieces[NUM_PREVIEW_PIECES-1] = CreatePiece((PieceType)(rand() % SHAPE_COUNT));
			m_previewPieces[NUM_PREVIEW_PIECES-1]->PlaceIntoPreview(NUM_PREVIEW_PIECES-1);

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
	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Escape;
		mapping.InputFunc = &Tetris::KeyExit;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::LAlt;
		mapping.InputFunc = &Tetris::KeyGarbage;
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
	AddGarbage(4);
}

void Tetris::KeyRotate()
{
	if (m_currentPiece)
	{
		m_currentPiece->Rotate(true);
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
	
	if (m_currentPiece != NULL)
	{
		m_currentPiece->m_originRowIdx += DROP_SPEED * dt;
		if (!m_currentPiece->PositionIsValid())
		{
			m_currentPiece->m_originRowIdx -= DROP_SPEED * dt;
			DropCurrentPiece();
		}		
	}

}

void Tetris::Draw(sf::RenderWindow* window)
{
	sf::Vector2f fieldOrigin(FIELD_ORIGIN_X,FIELD_ORIGIN_Y);
	sf::RectangleShape field;
	field.setSize(sf::Vector2f(FIELD_WIDTH,FIELD_HEIGHT));
	field.setPosition(fieldOrigin);
	field.setFillColor(sf::Color::Black);
	window->draw(field);

	// Preview
	sf::RectangleShape previewContainer;
	previewContainer.setSize(sf::Vector2f(GRID_SIZE * 5, GRID_SIZE * 5 * NUM_PREVIEW_PIECES));
	previewContainer.setOutlineThickness(10.0f);	
	previewContainer.setPosition(fieldOrigin + sf::Vector2f(FIELD_WIDTH + GRID_SIZE * 3, previewContainer.getOutlineThickness()));
	previewContainer.setFillColor(sf::Color(128, 128, 128, 255));
	previewContainer.setOutlineColor(sf::Color(70, 70, 70, 255));
	window->draw(previewContainer);

	sf::RectangleShape swapContainer;
	swapContainer.setSize(sf::Vector2f(GRID_SIZE * 5, GRID_SIZE * 5));
	swapContainer.setOutlineThickness(10.0f);
	swapContainer.setPosition(fieldOrigin + sf::Vector2f( - GRID_SIZE * 7, swapContainer.getOutlineThickness()));
	swapContainer.setFillColor(sf::Color(128, 128, 128, 255));
	swapContainer.setOutlineColor(sf::Color(70, 70, 70, 255));
	window->draw(swapContainer);

	for (int i=0 ; i < NUM_COLS ; i++)
	{
		for (int j=0 ; j < NUM_ROWS ; j++)
		{			
			window->draw(m_grid[i][j].m_backgroundShape);
			if (m_grid[i][j].m_isFilled)
			{
				m_grid[i][j].m_block.Draw(window, 0, 0, false);
			}
		}
	}

	if (m_currentPiece != NULL)
	{
		m_currentPiece->Draw(window, false);

		// Draw a ghost piece
		float curCol = m_currentPiece->m_originColIdx;
		float curRow = m_currentPiece->m_originRowIdx;		
		
		m_currentPiece->Move(0, NUM_ROWS);
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
}

void Tetris::DropCurrentPiece()
{
	m_currentPiece->Drop();
	CreateNewPiece(true);

	// Check for clears

	for (int i = 0; i < NUM_ROWS; i++)
	{
		bool clear = true;
		for (int j = 0; j < NUM_COLS; j++)
		{
			if (!m_grid[j][i].m_isFilled)
			{
				clear = false;
				break;
			}
		}

		if (clear)
		{			
			ClearRow(i);			
		}
	}
}

void Tetris::ClearRow(int row)
{
	// Clear this line
	for (int i = 0; i < NUM_COLS; i++)
	{
		m_grid[i][row].m_isFilled = false;
	}

	// Move every row above it down
	for (int i = row - 1; i >= 0; i--)
	{
		DropRow(i);
	}
}

void Tetris::DropRow(int row)
{
	if (row < NUM_ROWS)
	{
		for (int i = 0; i < NUM_COLS; i++)
		{
			if (m_grid[i][row].m_isFilled)
			{
				m_grid[i][row + 1].m_block.shape = m_grid[i][row].m_block.shape;
				m_grid[i][row + 1].m_isFilled = true;
				m_grid[i][row].m_isFilled = false;
			}
		}
	}
}

void Tetris::AddGarbage(int numRows)
{
	for (int i = 0; i < (NUM_ROWS - numRows); i++)
	{
		for (int k = 0; k < NUM_COLS; k++)
		{
			m_grid[k][i].m_isFilled = m_grid[k][i + numRows].m_isFilled;
			m_grid[k][i].m_block.shape = m_grid[k][i + numRows].m_block.shape;
		}
	}
	
	for (int i = (NUM_ROWS - numRows); i < NUM_ROWS; i++)
	{
		int holeIdx = rand() % NUM_COLS;
		for (int k = 0; k < NUM_COLS; k++)
		{
			// Pick a hole
			if (k != holeIdx)
			{
				m_grid[k][i].m_isFilled = true;
				FillPieceShape(m_grid[k][i].m_block.shape, sf::Color(255, 255, 255, 255), sf::Color(128, 128, 128, 255));
			}
			else
			{
				m_grid[k][i].m_isFilled = false;
			}
		}
	}
}

bool Tetris::IsRunning()
{
	return m_isRunning;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}


int main(int argc, char** argv)
{
	std::string ipAddress("");	

	if (argc > 1)
	{
		if (stricmp(argv[1], "--server") == 0)
		{
			g_networkPtr = new TetrisServer();			
		}
		else
		{
			ipAddress = argv[1];
			g_networkPtr = new TetrisClient(ipAddress);
		}
	}

	if (g_networkPtr != NULL)
	{
		if (!g_networkPtr->Init())
		{			
			delete g_networkPtr;
			g_networkPtr = NULL;
		}
	}
	
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML TETRIS");  

	Tetris game;
	game.Init();
	sf::Clock clock;

    while (window.isOpen() && game.IsRunning())
    {
		if (g_networkPtr != NULL)
		{ 
			g_networkPtr->Loop();			
		}

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

		window.clear(sf::Color(40,40,40));
		sf::Time time = clock.restart();
		game.Update(time.asSeconds());
		game.Draw(&window);
        window.display();
    }

	if (g_networkPtr != NULL)
	{
		delete g_networkPtr;
		g_networkPtr = NULL;		
	}
    return 0;
}

bool TetrisServer::Init()
{	
	m_interface = RakNet::RakPeerInterface::GetInstance();
	RakNet::RakNetStatistics *rss;
	m_interface->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	m_interface->SetTimeoutTime(30000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	// Record the first client that connects to us so we can pass it to the ping function
	m_clientID = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	// Holds user data
	char* portstring = "1234";

	printf("This is a sample implementation of a text based chat server.\n");
	printf("Connect to the project 'Chat Example Client'.\n");
	printf("Difficulty: Beginner\n\n");

	printf("Starting server.\n");
	// Starting the server is very simple.  2 players allowed.
	// 0 means we don't care about a connectionValidationInteger, and false
	// for low priority threads
	// I am creating two socketDesciptors, to create two sockets. One using IPV6 and the other IPV4
	RakNet::SocketDescriptor socketDescriptors[2];
	socketDescriptors[0].port = atoi(portstring);
	socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
	socketDescriptors[1].port = atoi(portstring);
	socketDescriptors[1].socketFamily = AF_INET6; // Test out IPV6
	bool b = m_interface->Startup(4, socketDescriptors, 2) == RakNet::RAKNET_STARTED;
	m_interface->SetMaximumIncomingConnections(4);
	if (!b)
	{
		printf("Failed to start dual IPV4 and IPV6 ports. Trying IPV4 only.\n");

		// Try again, but leave out IPV6
		b = m_interface->Startup(4, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
		if (!b)
		{
			printf("Server failed to start.  Terminating.\n");
			return false;
		}
	}
	m_interface->SetOccasionalPing(true);
	m_interface->SetUnreliableTimeout(1000);

	DataStructures::List< RakNet::RakNetSocket2* > sockets;
	m_interface->GetSockets(sockets);
	printf("Socket addresses used by RakNet:\n");
	for (unsigned int i = 0; i < sockets.Size(); i++)
	{
		printf("%i. %s\n", i + 1, sockets[i]->GetBoundAddress().ToString(true));
	}

	printf("\nMy IP addresses:\n");
	for (unsigned int i = 0; i < m_interface->GetNumberOfAddresses(); i++)
	{
		RakNet::SystemAddress sa = m_interface->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
		printf("%i. %s (LAN=%i)\n", i + 1, sa.ToString(false), sa.IsLANAddress());
	}

	printf("\nMy GUID is %s\n", m_interface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());

	return true;
}

TetrisServer::~TetrisServer()
{	
	m_interface->Shutdown(300);
	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(m_interface);
}

void TetrisServer::Loop()
{
	// This sleep keeps RakNet responsive
	RakSleep(30);
	
	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	for (p = m_interface->Receive(); p; m_interface->DeallocatePacket(p), p = m_interface->Receive())
	{
		// We got a packet, get the identifier with our handy function
		packetIdentifier = GetPacketIdentifier(p);

		// Check if this is a network message packet
		switch (packetIdentifier)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString(true));;
			break;

		case ID_NEW_INCOMING_CONNECTION:
			// Somebody connected.  We have their IP now
			printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
			m_clientID = p->systemAddress; // Record the player ID of the client

			printf("Remote internal IDs:\n");
			for (int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++)
			{
				RakNet::SystemAddress internalId = m_interface->GetInternalID(p->systemAddress, index);
				if (internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				{
					printf("%i. %s\n", index + 1, internalId.ToString(true));
				}
			}

			break;

		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;

		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", p->systemAddress.ToString(true));
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString(true));;
			break;

		default:
			break;
		}
	}
}

TetrisClient::TetrisClient(std::string & ipAddr) :
	m_ipAddress(ipAddr)
{
}

TetrisClient::~TetrisClient()
{
	RakNet::RakPeerInterface *client = RakNet::RakPeerInterface::GetInstance();

	// Be nice and let the server know we quit.
	m_interface->Shutdown(300);

	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(client);

}

bool TetrisClient::Init()
{
	RakNet::RakNetStatistics *rss;
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	m_interface = RakNet::RakPeerInterface::GetInstance();
	//	m_interface->InitializeSecurity(0,0,0,0);
	//RakNet::PacketLogger packetLogger;
	//m_interface->AttachPlugin(&packetLogger);

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Just so we can remember where the packet came from
	bool isServer;

	// Record the first client that connects to us so we can pass it to the ping function
	m_clientID = RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	// Crude interface

	// Holds user data
	char ip[64], serverPort[30], clientPort[30];
	
	printf("This is a sample implementation of a text based chat client.\n");
	printf("Connect to the project 'Chat Example Server'.\n");
	printf("Difficulty: Beginner\n\n");

	// Get our input
	strcpy(clientPort, "5");
	strcpy(ip, "127.0.0.1");	
	strcpy(serverPort, "1234");

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	RakNet::SocketDescriptor socketDescriptor(atoi(clientPort), 0);
	socketDescriptor.socketFamily = AF_INET;
	m_interface->Startup(8, &socketDescriptor, 1);
	m_interface->SetOccasionalPing(true);

	RakNet::ConnectionAttemptResult car = m_interface->Connect(ip, atoi(serverPort), "Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);

	printf("\nMy IP addresses:\n");
	unsigned int i;
	for (i = 0; i < m_interface->GetNumberOfAddresses(); i++)
	{
		printf("%i. %s\n", i + 1, m_interface->GetLocalIP(i));
	}

	printf("My GUID is %s\n", m_interface->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());

	return true;
}

void TetrisClient::Loop()
{
	// Get a packet from either the server or the client
	RakNet::RakPeerInterface *client = RakNet::RakPeerInterface::GetInstance();

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	for (p = m_interface->Receive(); p; m_interface->DeallocatePacket(p), p = m_interface->Receive())
	{
		// We got a packet, get the identifier with our handy function
		packetIdentifier = GetPacketIdentifier(p);

		// Check if this is a network message packet
		switch (packetIdentifier)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_ALREADY_CONNECTED:
			// Connection lost normally
			printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", p->guid);
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_CONNECTION_LOST\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
			break;
		case ID_CONNECTION_BANNED: // Banned from this server
			printf("We are banned from this server.\n");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("Connection attempt failed\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			// Sorry, the server is full.  I don't do anything here but
			// A real app should tell the user
			printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			break;

		case ID_INVALID_PASSWORD:
			printf("ID_INVALID_PASSWORD\n");
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST\n");
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			// This tells the client they have connected
			printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
			printf("My external address is %s\n", m_interface->GetExternalID(p->systemAddress).ToString(true));
			break;
		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", p->systemAddress.ToString(true));
			break;
		default:
			// It's a client, so just show the message
			printf("%s\n", p->data);
			break;
		}
	}
}
