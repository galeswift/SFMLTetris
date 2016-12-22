#pragma once

#include "Constants.h"

class Tetris;

class CombatUtil
{
public:
	static void Attack(Tetris* owner, s32 dmg); // attacks everyone
};