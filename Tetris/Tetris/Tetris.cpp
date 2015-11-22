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
void Piece::Rotate(bool right)
{
	for (int i = 0; i < 4; i++)
	{
		int oldCol = m_blocks[i].colIdx;
		m_blocks[i].colIdx = -m_blocks[i].rowIdx;
		m_blocks[i].rowIdx = oldCol;		
	}

	if (!PositionIsValid())
	{
		// Undo the rotation
		for (int i = 0; i < 4; i++)
		{
			int oldCol = m_blocks[i].colIdx;
			m_blocks[i].colIdx = m_blocks[i].rowIdx;
			m_blocks[i].rowIdx = -oldCol;
		}
	}
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

	// Now check each piece aganst every other piece
	
			
	
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
		break;
	}
	
	result->m_originColIdx = (int)(NUM_COLS * 0.5f) -1;
	result->m_originRowIdx = 0;

	for (int i = 0; i < 4; i++)
	{
		sf::RectangleShape& grid = result->m_blocks[i].shape;
		grid.setFillColor(sf::Color(170, 57, 57, 255));
		grid.setOutlineColor(sf::Color(128, 21, 21, 255));
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
	m_dropTimer = DROP_SPEED;
	m_inputTimer = INPUT_REPEAT_DELAY;
}

void Tetris::Update(float dt)
{
	m_dropTimer -= dt;
	m_inputTimer -= dt;

	if (m_inputTimer <= 0.0f)
	{
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
		{			
			m_inputTimer = INPUT_REPEAT_DELAY;
			if (m_currentPiece)
			{
				m_currentPiece->Rotate(true);
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			m_inputTimer = INPUT_REPEAT_DELAY;
			if (m_currentPiece)
			{
				m_currentPiece->Move(1,0);
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			m_inputTimer = INPUT_REPEAT_DELAY;
			if (m_currentPiece)
			{
				m_currentPiece->Move(-1, 0);
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			m_inputTimer = INPUT_REPEAT_DELAY;
			if (m_currentPiece)
			{
				if (!m_currentPiece->Move(0, 1))
				{				
					DropCurrentPiece();					
				}
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_inputTimer = 0.5f;
			DropCurrentPiece();
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
			m_grid[i][j].m_block.Draw(window, 0, 0);
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