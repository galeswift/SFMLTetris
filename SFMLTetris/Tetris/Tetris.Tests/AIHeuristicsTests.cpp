#include "stdafx.h"
#include "catch.hpp"
#include "TestHelpers.h"
#include "AIHeuristics.h"

TEST_CASE("AggregateHeight sums the height of every column", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_AggregateHeight heuristic(1.0f);

	SECTION("empty board scores zero")
	{
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("single block on the floor counts as height one")
	{
		FillColumn(board, 0, 1);
		REQUIRE(heuristic.GetScore(&original, &board) == 1.0f);
	}

	SECTION("heights accumulate across columns")
	{
		FillColumn(board, 0, 3);
		FillColumn(board, 4, 5);
		REQUIRE(heuristic.GetScore(&original, &board) == 8.0f);
	}

	SECTION("only the topmost block of a column matters")
	{
		// A floating block above an empty gap still defines the column height.
		FillCell(board, 2, board.m_rows - 4);
		REQUIRE(heuristic.GetScore(&original, &board) == 4.0f);
	}

	SECTION("scalar weights the result")
	{
		AIHeuristic_AggregateHeight weighted(-0.5f);
		FillColumn(board, 0, 4);
		REQUIRE(weighted.GetScore(&original, &board) == -2.0f);
	}
}

TEST_CASE("Holes counts empty cells below a filled cell", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_Holes heuristic(1.0f);

	SECTION("empty board has no holes")
	{
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("solid column has no holes")
	{
		FillColumn(board, 0, 5);
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("a covered gap is a hole")
	{
		FillCell(board, 0, board.m_rows - 2); // cover
		// bottom cell left empty -> one hole
		REQUIRE(heuristic.GetScore(&original, &board) == 1.0f);
	}

	SECTION("every empty cell under the cover counts")
	{
		FillCell(board, 3, board.m_rows - 5);
		REQUIRE(heuristic.GetScore(&original, &board) == 4.0f);
	}
}

TEST_CASE("Bumpiness sums height differences of adjacent columns", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_Bumpiness heuristic(1.0f);

	SECTION("flat board is not bumpy")
	{
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);

		for (int col = 0; col < board.m_cols; col++)
		{
			FillColumn(board, col, 2);
		}
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("a single tower is counted on the way up and down")
	{
		FillColumn(board, 4, 3);
		REQUIRE(heuristic.GetScore(&original, &board) == 6.0f);
	}

	SECTION("staircase accumulates each step")
	{
		FillColumn(board, 0, 1);
		FillColumn(board, 1, 2);
		FillColumn(board, 2, 3);
		// steps: |0->1|=1 (implicit col before 0 not counted), |1-2|=1, |2-3|=1, |3-0|=3
		REQUIRE(heuristic.GetScore(&original, &board) == 5.0f);
	}
}

TEST_CASE("CompletedLines scores rows cleared relative to the original board", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_CompletedLines heuristic(2.0f);

	original.m_clearedRows = 3;
	board.m_clearedRows = 5;
	REQUIRE(heuristic.GetScore(&original, &board) == 4.0f);

	board.m_clearedRows = 3;
	REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
}

TEST_CASE("GameLoss heavily penalizes a board reset", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_GameLoss heuristic(1.0f);

	SECTION("no reset, no penalty")
	{
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("reset costs a hundred million")
	{
		board.m_resetCount = original.m_resetCount + 1;
		REQUIRE(heuristic.GetScore(&original, &board) == -100000000.0f);
	}
}

TEST_CASE("HighestCol reports the tallest column with a danger bonus near the top", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_HighestCol heuristic(1.0f);

	SECTION("safe height is reported as-is")
	{
		FillColumn(board, 1, 5);
		REQUIRE(heuristic.GetScore(&original, &board) == 5.0f);
	}

	SECTION("height within six rows of the ceiling gets +6")
	{
		int dangerHeight = board.m_rows - 3;
		FillColumn(board, 1, dangerHeight);
		REQUIRE(heuristic.GetScore(&original, &board) == (float)(dangerHeight + 6));
	}
}

TEST_CASE("Blockade counts the filled cells stacked above a hole", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_Blockade heuristic(1.0f);

	SECTION("solid column with no hole is not a blockade")
	{
		FillColumn(board, 0, 4);
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("blocks covering a hole are counted")
	{
		// Two filled cells, then a gap, then the floor.
		FillCell(board, 0, board.m_rows - 4);
		FillCell(board, 0, board.m_rows - 3);
		REQUIRE(heuristic.GetScore(&original, &board) == 2.0f);
	}
}

TEST_CASE("DeepestHole finds the deepest well flanked by walls or blocks", "[heuristics]")
{
	Tetris original, board;
	InitBareBoard(original);
	InitBareBoard(board);
	AIHeuristic_DeepestHole heuristic(1.0f);

	SECTION("empty board has no well")
	{
		REQUIRE(heuristic.GetScore(&original, &board) == 0.0f);
	}

	SECTION("a well against the wall is measured by its flanked depth")
	{
		// Column 1 filled 4 high, column 0 empty: column 0 is a well of
		// depth 4 (board wall on the left, blocks on the right).
		FillColumn(board, 1, 4);
		REQUIRE(heuristic.GetScore(&original, &board) == 4.0f);
	}
}
