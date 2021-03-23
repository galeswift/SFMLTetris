#pragma once

#define NUM_COLS 10
#define NUM_ROWS 22
#define GRID_SIZE 30
#define FIELD_ORIGIN_X 150
#define FIELD_ORIGIN_Y 150
#define PLAYER_SCALE 0.75f
#define DROP_SPEED 0.5f
#define MAX_DROP_SPEED 16.0f
#define NUM_PREVIEW_PIECES 3
#define PREVIEW_PANEL_SIZE (GRID_SIZE * 5)
#define SWAP_PANEL_SIZE (GRID_SIZE * 5)
#define INPUT_REPEAT_START_DELAY 0.5f
#define INPUT_REPEAT_INTERVAL 0.1f
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1024
#define WINDOW_CLEAR_COLOR (sf::Color(212, 212, 212))
#define BLOCK_OUTLINE_THICKNESS (-2.0f)
#define BLOCK_MARGIN_THICKNESS 5.0f
#define FIELD_OUTLINE_COLOR (sf::Color(5, 5, 5))
#define FIELD_OUTLINE_THICKNESS 6.0f
#define GRID_LINE_COLOR (sf::Color(212, 212, 212, 150))
#define GRID_FILL_COLOR (sf::Color(250, 250, 250))

typedef float f32;
typedef double f64;
typedef int64_t s64;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef u32 GameHandle;

