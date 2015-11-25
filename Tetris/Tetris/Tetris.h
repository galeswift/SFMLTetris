#include <SFML/Graphics.hpp>

#define NUM_COLS 10
#define NUM_ROWS 22
#define GRID_SIZE 20
#define FIELD_WIDTH (GRID_SIZE * NUM_COLS)
#define FIELD_HEIGHT (GRID_SIZE * NUM_ROWS)
#define FIELD_ORIGIN_X 300
#define FIELD_ORIGIN_Y 100
#define DROP_SPEED 0.5f
#define NUM_PREVIEW_PIECES 3
#define INPUT_REPEAT_START_DELAY 0.5f
#define INPUT_REPEAT_INTERVAL 0.1f

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
	void Draw(sf::RenderWindow* window, int colOffset, int rowOffset, bool ghost);
};

class Piece
{
public:
	void Draw(sf::RenderWindow* window, bool ghost);
	bool Rotate(bool right);
	void Init(Tetris* board);
	bool Move(int x, int y);
	bool PositionIsValid();
	void Drop();	
	void PlaceIntoHolding();
	void PlaceIntoGrid();
	void PlaceIntoPreview(int slot);

	Tetris* m_board;
	Block m_blocks[4];
	float m_originRowIdx;
	float m_originColIdx;
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
	InputMapping() : m_timeHeld(0.0f)
	{

	}
	sf::Keyboard::Key key;
	float m_timeHeld;
	bool m_isHeld;
	void(Tetris::*InputFunc)();
};

class Tetris
{
public:
	Tetris();
	Piece* CreatePiece(PieceType type);
	void Init();
	void Reset();
	void CreateNewPiece(bool deleteCurrent);
	void InitKeyBindings();
	void Update(float dt);
	void Draw(sf::RenderWindow* window);
	void DropCurrentPiece();
	void ClearRow(int row);
	void DropRow(int row);	
	bool IsRunning();

	// Input mappings
	void KeyExit();
	void KeyRotate();
	void KeyMoveLeft();
	void KeyMoveRight();
	void KeyMoveDown();
	void KeyDrop();
	void KeySwap();

	GridCell m_grid[NUM_COLS][NUM_ROWS];
	Piece* m_currentPiece;		
	Piece* m_heldPiece;
	Piece* m_previewPieces[NUM_PREVIEW_PIECES];
	float m_repeatTimer;
	float m_repeatStartTimer;
	bool m_canSwapPiece;
	bool m_isRunning;
	std::vector<InputMapping> m_inputs;
};

int main();
