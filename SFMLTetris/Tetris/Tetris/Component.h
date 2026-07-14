#pragma once

class Tetris;

enum ComponentType
{
	COMPONENT_PLAYER,
	COMPONENT_SOUND,
	COMPONENT_COMBAT,
	COMPONENT_AI_CONTROLLER,
	COMPONENT_AI_EVALUATOR,
	COMPONENT_AI_SPAWN,
	COMPONENT_TYPE_COUNT
};

class Component
{
public:
	Component() : m_owner(NULL) {}
	virtual ~Component() {};
	// Called when the component is handed out from a pool; clear any per-game state here
	virtual void Reset() {};
	Tetris* m_owner;
};

// Goes in the component class body. Pairs with DEFINE_COMPONENT in the .cpp,
// which creates the pool this type id maps to.
#define DECLARE_COMPONENT(enumType) \
public: \
	static const ComponentType TYPE = enumType;
