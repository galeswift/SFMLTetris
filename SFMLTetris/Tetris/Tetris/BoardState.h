#pragma once

class BoardState;

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
	void Draw(sf::RenderWindow* window, const sf::Vector2f& vecOrigin, sf::RectangleShape& blockShape, int colOffset, int rowOffset, bool ghost);
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
	bool Rotate(bool right);
	void Init(BoardState* board);
	bool Move(int x, int y);
	bool PositionIsValid();
	void Drop();
	void PlaceIntoHolding();
	void PlaceIntoGrid();
	void PlaceIntoPreview(int slot);
	int MaxRotations() { return m_maxRotations;  }
	BoardState* m_board;
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

class TetrisGrid
{
public:
	GridCell m_cells[NUM_COLS][NUM_ROWS];
};

// Pure game state and rules - no rendering, input, or components.
// The AI clones and simulates these directly.
class BoardState
{
public:
	BoardState();
	virtual ~BoardState();

	void InitBoard(int rows, int cols);
	void ResetBoard();
	void Clone(const BoardState* other);

	Piece* CreatePiece(PieceType type);
	static void FillPieceShape(Block* block, const sf::Color& pieceColor, const sf::Color& outlineColor);
	void CreateNewPiece(bool deleteCurrent);
	void DropCurrentPiece();
	void ClearRow(int row);
	void DropRow(int row);
	void AddGarbage(int numRows);
	void UpdateCurrentLevel();
	void OnBoardFilled();
	bool IsBoardPlayable();
	float GetDropSpeed();

	// Piece actions - shared by player input, the AI controller, and AI simulation
	bool RotateCurrentPiece();
	void SwapPiece();

	TetrisGrid m_grid;
	Piece* m_currentPiece;
	Piece* m_heldPiece;
	Piece* m_previewPieces[NUM_PREVIEW_PIECES];

	float m_levelDropSpeed;
	bool m_canSwapPiece;
	bool m_boardIsPlayable;
	bool m_isClone;
	int m_clearedRows;
	int m_currentLevel;
	int m_pieceID;
	int m_rows;
	int m_cols;
	int m_resetCount;
	int m_lastClearCount;
};
