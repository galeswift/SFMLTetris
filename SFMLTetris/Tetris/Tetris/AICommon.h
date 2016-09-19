#pragma once

#define DEFAULT_SCORE -99999
class DesiredMoveSet
{
public:
	DesiredMoveSet()
		: score(DEFAULT_SCORE)
		, id(-1)
		, numRotations(0)
		, col(0)
		, used(false)
	{

	}


	float score;
	int id;
	int numRotations;
	int col;
	bool used;
};
