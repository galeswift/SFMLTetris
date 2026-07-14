#pragma once

#include "BoardState.h"
#include "Component.h"

class GameManager;

// Every way to poke a board goes through this - keyboard, AI controller,
// debug keys, and (someday) the network all issue the same commands.
enum InputCommand
{
	INPUT_EXIT,
	INPUT_GARBAGE,
	INPUT_CLEAR_ROW,
	INPUT_ROTATE,
	INPUT_MOVE_LEFT,
	INPUT_MOVE_RIGHT,
	INPUT_MOVE_DOWN,
	INPUT_DROP,
	INPUT_SWAP
};

struct InputMapping
{
	InputMapping() : m_timeHeld(0.0f)
	{

	}
	sf::Keyboard::Key key;
	float m_timeHeld;
	bool m_isHeld;
	InputCommand command;
};

// Presentation and input on top of a BoardState
class Tetris : public BoardState
{
public:
	Tetris();
	virtual ~Tetris();

	void Init(GameManager* manager, bool isPlayer, int rows, int cols);
	void InitResources();
	void InitKeyBindings();
	void Reset();
	void Update(float dt);
	void Draw(sf::RenderWindow* window);
	void DrawPiece(sf::RenderWindow* window, Piece* piece, bool ghost);
	void ApplyInput(InputCommand command);
	void QueueSound(const char* soundName);
	bool IsRunning();

	int GetTotalGameHeight();
	int GetTotalGameWidth();
	const sf::RectangleShape* GetFieldShape();

	template<typename T>
	T* GetComponent() { return (T*)m_componentSlots[T::TYPE]; }

	GameManager* m_manager;
	Component* m_componentSlots[COMPONENT_TYPE_COUNT];

	sf::RectangleShape m_blockShape;
	sf::RectangleShape m_fieldShape;
	sf::RectangleShape m_previewContainerShape;
	sf::RectangleShape m_swapContainerShape;

	sf::Vector2f m_fieldOrigin;
	int	m_fieldWidth;
	int	m_fieldHeight;
	int m_totalGameWidth;
	int m_totalGameHeight;
	bool m_isRunning;

	sf::Font m_mainFont;
	sf::Font m_debugFont;
	std::vector<InputMapping> m_inputs;
};
