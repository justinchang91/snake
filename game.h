#ifndef GAME_H
#define GAME_H

#include "ring_buffer.h"

#define NUMROWS 10
#define NUMCOLS 10
#define OCCUPIED -1
#define MAX_SCORE (NUMROWS * NUMCOLS - 1)  // Max number of food eaten

typedef struct {
    coordinate_t buf[MAX_SCORE];
    int size;
} available_cells_t;

typedef struct {
    snake_buffer_t sb;
    int occupancy_grid[NUMROWS][NUMCOLS];
    available_cells_t available_cells;
    coordinate_t food_pos;
    coordinate_t direction;
    int score;
    int game_started;

} game_t;
#endif