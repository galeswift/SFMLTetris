#include "Constants.h"

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
	Block()
		: rowIdx(-1)
		, colIdx(-1)		
	{

	}

	sf::Color m_fillColor;
	sf::Color m_outlineColor;

	int rowIdx;
	int colIdx;		
	void Draw(sf::RenderWindow* window, sf::RectangleShape& blockShape, int colOffset, int rowOffset, bool ghost);
};

class Piece
{
public:
	Piece() 
	: m_board(NULL)
	, m_originRotations(0)
	, m_id(0)
	{};
	Piece(const Piece& other);
	void Draw(sf::RenderWindow* window, bool ghost);
	bool Rotate(bool right);
	void Init(Tetris* board);
	bool Move(int x, int y);
	bool PositionIsValid();
	void Drop();	
	void PlaceIntoHolding();
	void PlaceIntoGrid();
	void PlaceIntoPreview(int slot);
	int MaxRotations() { return m_maxRotations;  }
	Tetris* m_board;
	Block m_blocks[4];
	float m_originRowIdx;
	float m_originColIdx;
	int m_pivotRow;
	int m_pivotCol;
	int m_maxRotations;
	int m_originRotations;
	int m_id;
	PieceType m_type;

private:
	bool __TryRotate(bool right);
	void __DoRotate(bool right);
};

class GridCell
{
public:
	GridCell()		
		: m_isFilled(false)
	{

	}
	Block m_block;	
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

class TetrisGrid
{
public:
	sf::RectangleShape m_backgroundShape;
	GridCell m_cells[NUM_COLS][NUM_ROWS];
};

class Tetris
{
public:
	Tetris();
	~Tetris();
	void Clone(const Tetris* other);
	Piece* CreatePiece(PieceType type);
	void FillPieceShape(Block* block, sf::Color & pieceColor, sf::Color & outlineColor);

	void Init(bool isPlayer, int rows, int cols);
	void InitResources();
	void InitKeyBindings();
	void Reset();
	void CreateNewPiece(bool deleteCurrent);
	void Update(float dt);
	void Draw(sf::RenderWindow* window);
	void DropCurrentPiece();
	void ClearRow(int row);
	void UpdateCurrentLevel();
	void DropRow(int row);	
	void AddGarbage(int numRows);
	bool IsRunning();
	float GetDropSpeed();
	// Input mappings
	void KeyExit();
	void KeyGarbage();
	void KeyClearRow();
	void KeyRotate();
	void KeyMoveLeft();
	void KeyMoveRight();
	void KeyMoveDown();
	void KeyDrop();
	void KeySwap();

	TetrisGrid m_grid;	
	Piece* m_currentPiece;		
	Piece* m_heldPiece;
	Piece* m_previewPieces[NUM_PREVIEW_PIECES];
	sf::RectangleShape m_blockShape;	
	float m_repeatTimer;
	float m_repeatStartTimer;
	float m_levelDropSpeed;
	bool m_canSwapPiece;
	bool m_isRunning;
	bool m_isClone;
	int m_clearedRows;
	int m_currentLevel;
	int m_pieceID;
	int m_rows;
	int m_cols;
	int m_resetCount;

	sf::Font m_mainFont;
	sf::Font m_debugFont;
	std::vector<InputMapping> m_inputs;
};
