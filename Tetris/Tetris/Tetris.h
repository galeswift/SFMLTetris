#include <SFML/Graphics.hpp>

#define NUM_COLS 8
#define NUM_ROWS 16
#define FIELD_WIDTH 200
#define FIELD_HEIGHT 400
#define GRID_SIZE (FIELD_WIDTH/NUM_COLS)
#define FIELD_ORIGIN_X 300
#define FIELD_ORIGIN_Y 100
#define DROP_SPEED 0.5f
#define INPUT_REPEAT_DELAY 0.16f

class Tetris;

enum PieceType
{
	SHAPE_T,
	SHAPE_Z,
	SHAPE_S,
	SHAPE_L,
	SHAPE_J,
	SHAPE_CUBE,
	SHAPE_TETRIS,
	SHAPE_COUNT
};

class Piece;

class Block
{
public:	
	int rowIdx;
	int colIdx;
	sf::RectangleShape shape;
	void Draw(sf::RenderWindow* window, int colOffset, int rowOffset);
};

class Piece
{
public:
	Block m_blocks[4];
	void Draw(sf::RenderWindow* window);
	float m_originRowIdx;
	float m_originColIdx;
	void Init(Tetris* board);
	void Rotate(bool right);
	bool Move(int x, int y);
	bool PositionIsValid();
	void Drop();

	Tetris* m_board;
};

class GridCell
{
public:
	Block m_block;
	sf::RectangleShape m_backgroundShape;
	bool m_isFilled;
};

class Tetris
{
public:
	Piece* CreatePiece(PieceType type);
	void Init();
	void Update(float dt);
	void Draw(sf::RenderWindow* window);
	void DropCurrentPiece();
	GridCell m_grid[NUM_COLS][NUM_ROWS];
	Piece* m_currentPiece;
	float m_dropTimer;
	float m_inputTimer;
};