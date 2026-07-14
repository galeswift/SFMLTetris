#pragma once

class GameManager;
class Tetris;

class CombatUtil
{
public:
	static void Attack(GameManager& manager, Tetris* owner, s32 dmg); // attacks everyone
};
