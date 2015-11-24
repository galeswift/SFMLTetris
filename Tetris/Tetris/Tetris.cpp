// Tetris.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "Tetris.h"

void Block::Draw(sf::RenderWindow * window, int colOffset, int rowOffset)
{
	sf::Vector2f fieldOrigin(FIELD_ORIGIN_X, FIELD_ORIGIN_Y);	
	sf::Vector2f position = fieldOrigin;
	position += sf::Vector2f((colOffset + colIdx) * GRID_SIZE, (rowOffset + rowIdx)*GRID_SIZE);
	shape.setPosition(position);
	window->draw(shape);
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

void Piece::Draw(sf::RenderWindow* window)
{
	for (int i=0 ; i<4 ; i++)
	{
		m_blocks[i].Draw(window, m_originColIdx, m_originRowIdx);
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
	
	result->m_originColIdx = (int)(NUM_COLS * 0.5f) -1;
	result->m_originRowIdx = 2;

	sf::Color outlineColor = pieceColor;
	outlineColor.r *= 0.75f;
	outlineColor.g *= 0.75f;
	outlineColor.b *= 0.75f;
	for (int i = 0; i < 4; i++)
	{
		sf::RectangleShape& grid = result->m_blocks[i].shape;
		grid.setFillColor(pieceColor);
		grid.setOutlineColor(outlineColor);
		grid.setOutlineThickness(2.0f);
		grid.setSize(sf::Vector2f(GRID_SIZE - grid.getOutlineThickness(), GRID_SIZE - grid.getOutlineThickness()));
	}
	return result;
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
			grid.setOutlineColor(sf::Color::White);
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

	m_currentPiece = CreatePiece((PieceType)(rand() % SHAPE_COUNT));	
	m_repeatTimer = INPUT_REPEAT_INTERVAL;

	InitKeyBindings();
}

void Tetris::InitKeyBindings()
{
	// Input bindings
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

	for (int i=0 ; i < NUM_COLS ; i++)
	{
		for (int j=0 ; j < NUM_ROWS ; j++)
		{			
			window->draw(m_grid[i][j].m_backgroundShape);
			if (m_grid[i][j].m_isFilled)
			{
				m_grid[i][j].m_block.Draw(window, 0, 0);
			}
		}
	}

	if (m_currentPiece != NULL)
	{
		m_currentPiece->Draw(window);
	}
}

void Tetris::DropCurrentPiece()
{
	m_currentPiece->Drop();
	delete m_currentPiece;
	m_currentPiece = CreatePiece((PieceType)(rand() % SHAPE_COUNT));

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

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");  

	Tetris game;
	game.Init();
	sf::Clock clock;

    while (window.isOpen())
    {
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

    return 0;
}