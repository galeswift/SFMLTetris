#include <SFML/Graphics.hpp>

#define NUM_COLS 10
#define NUM_ROWS 22
#define FIELD_WIDTH 200
#define FIELD_HEIGHT 400
#define GRID_SIZE (FIELD_WIDTH/NUM_COLS)
#define FIELD_ORIGIN_X 300
#define FIELD_ORIGIN_Y 100
#define DROP_SPEED 0.5f
#define INPUT_REPEAT_START_DELAY 0.25f;
#define INPUT_REPEAT_INTERVAL 0.05f

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
	bool Rotate(bool right);
	void Init(Tetris* board);
	bool Move(int x, int y);
	bool PositionIsValid();
	void Drop();

	Tetris* m_board;
	int m_pivotRow;
	int m_pivotCol;

	PieceType m_type;
private:
	bool __TryRotate(bool right);
	void __DoRotate(bool right);
};

class GridCell
{
public:
	Block m_block;
	sf::RectangleShape m_backgroundShape;
	bool m_isFilled;
};

struct InputMapping
{
	sf::Keyboard::Key key;
	float m_timeHeld;
	void(Tetris::*InputFunc)();
};

class Tetris
{
public:
	Piece* CreatePiece(PieceType type);
	void Init();
	void Update(float dt);
	void Draw(sf::RenderWindow* window);
	void DropCurrentPiece();
	void ClearRow(int row);
	void DropRow(int row);

	GridCell m_grid[NUM_COLS][NUM_ROWS];
	Piece* m_currentPiece;
	float m_dropTimer;
	float m_repeatTimer;
	float m_repeatStartTimer;

	// Input mappings
	void KeyRotate();

	std::vector<InputMapping> m_inputs;
};