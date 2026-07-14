// Tetris.cpp : Presentation and input layer on top of BoardState
//

#include "stdafx.h"

// Other includes
#include <SFML/Graphics.hpp>
#include "ClientGame.h"
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

Tetris::Tetris()
	: m_manager(NULL)
	, m_isRunning(true)
{
	for (int i = 0; i < COMPONENT_TYPE_COUNT; i++)
	{
		m_componentSlots[i] = NULL;
	}
}

Tetris::~Tetris()
{
}

void Tetris::Init(GameManager* manager, bool isPlayer, int rows, int cols)
{
	m_manager = manager;

	InitBoard(rows, cols);
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
	ResetBoard();

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

void Tetris::InitKeyBindings()
{
	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::LAlt;
		mapping.command = INPUT_GARBAGE;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::RAlt;
		mapping.command = INPUT_CLEAR_ROW;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Up;
		mapping.command = INPUT_ROTATE;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Right;
		mapping.command = INPUT_MOVE_RIGHT;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Left;
		mapping.command = INPUT_MOVE_LEFT;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Down;
		mapping.command = INPUT_MOVE_DOWN;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::Space;
		mapping.command = INPUT_DROP;
		m_inputs.push_back(mapping);
	}

	{
		InputMapping mapping;
		mapping.key = sf::Keyboard::LControl;
		mapping.command = INPUT_SWAP;
		m_inputs.push_back(mapping);
	}
}

void Tetris::ApplyInput(InputCommand command)
{
	switch (command)
	{
	case INPUT_EXIT:
		m_isRunning = false;
		break;
	case INPUT_GARBAGE:
		CombatUtil::Attack(*m_manager, this, 4);
		break;
	case INPUT_CLEAR_ROW:
		ClearRow(m_rows - 1);
		break;
	case INPUT_ROTATE:
		if (RotateCurrentPiece())
		{
			QueueSound("Content/Sounds/block_rotate.wav");
		}
		break;
	case INPUT_MOVE_LEFT:
		if (m_currentPiece)
		{
			m_currentPiece->Move(-1, 0);
		}
		break;
	case INPUT_MOVE_RIGHT:
		if (m_currentPiece)
		{
			m_currentPiece->Move(1, 0);
		}
		break;
	case INPUT_MOVE_DOWN:
		if (m_currentPiece && !m_currentPiece->Move(0, 1))
		{
			DropCurrentPiece();
		}
		break;
	case INPUT_DROP:
		if (m_currentPiece)
		{
			QueueSound("Content/Sounds/block_drop.wav");
			DropCurrentPiece();
		}
		break;
	case INPUT_SWAP:
		SwapPiece();
		break;
	}
}

void Tetris::QueueSound(const char* soundName)
{
	SoundComponent* sound = GetComponent<SoundComponent>();
	if (sound != NULL)
	{
		sound->QueueSound(soundName);
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
				ApplyInput(mapping.command);

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

void Tetris::DrawPiece(sf::RenderWindow* window, Piece* piece, bool ghost)
{
	sf::Vector2f vecOrigin = m_fieldShape.getPosition();
	for (int i = 0; i < 4; i++)
	{
		piece->m_blocks[i].Draw(window, vecOrigin, m_blockShape, piece->m_originColIdx, piece->m_originRowIdx, ghost);
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
		DrawPiece(window, m_currentPiece, false);

		// Draw a ghost piece
		float curCol = m_currentPiece->m_originColIdx;
		float curRow = m_currentPiece->m_originRowIdx;

		m_currentPiece->Move(0, m_rows);
		DrawPiece(window, m_currentPiece, true);

		m_currentPiece->m_originColIdx = curCol;
		m_currentPiece->m_originRowIdx = curRow;
	}

	if (m_heldPiece != NULL)
	{
		DrawPiece(window, m_heldPiece, false);
	}

	// Draw preview
	for (int i = 0; i < NUM_PREVIEW_PIECES; i++)
	{
		if (m_previewPieces[i] != NULL)
		{
			DrawPiece(window, m_previewPieces[i], false);
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

bool Tetris::IsRunning()
{
	return m_isRunning;
}
