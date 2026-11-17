#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "ring_buffer.h"

#define NUMROWS 10
#define NUMCOLS 10
#define MAX_SCORE (NUMROWS * NUMCOLS - 1)  // Max number of food eaten

#define GOING_DOWN (*dx == 0 && *dy == 1)
#define GOING_UP (*dx == 0 && *dy == -1)
#define GOING_LEFT (*dx == -1 && *dy == 0)
#define GOING_RIGHT (*dx == 1 && *dy == 0)

_Static_assert(NUMROWS * NUMCOLS < RB_SIZE, "RB_SIZE too small for board");

int is_snake(int occupancy_grid[NUMROWS][NUMCOLS], const int x, const int y) 
{
    return occupancy_grid[y][x] == 1;
}

int is_food(const coordinate_t food_pos, const int x, const int y)
{
    return (y == food_pos.y && x == food_pos.x);
}

void print_board(int occupancy_grid[NUMROWS][NUMCOLS], const coordinate_t food_pos)
{
    move(0, 0);
    for (int r = 0; r < NUMROWS; r++) {
        for (int c = 0; c < NUMCOLS; c++) {
            if (is_snake(occupancy_grid, c, r)) {
                printw("@ ");
            }
            else if (is_food(food_pos, c, r)) {
                printw("* ");
            }
            else {
                printw(". ");
            }
        }
        printw("\n");
    }
}

void print_score(int *score) {
    mvprintw(NUMROWS + 1, 0, "Score: %d", *score);
}

void print_game_over(int *score) {
    if (*score == MAX_SCORE) {
        mvprintw(NUMROWS + 2, 0, "You won! Press any key to exit.");
    } else {
        mvprintw(NUMROWS + 2, 0, "Game over! Press any key to exit.");
    }
    
}

int in_bounds(const coordinate_t snake_head_pos) 
{
    return snake_head_pos.x > -1 && snake_head_pos.x < NUMCOLS && snake_head_pos.y > -1 && snake_head_pos.y < NUMROWS;
}

int food_is_touched(const coordinate_t snake_head_pos, const coordinate_t food_pos)
{
    return snake_head_pos.x == food_pos.x && snake_head_pos.y == food_pos.y;
}

void update_direction(int *dx, int *dy, const int ch, const snake_buffer_t *sb, int *gameStarted) 
{
    if ((ch == 'w' || ch == KEY_UP) && !(sb_get_size(sb) > 1 && GOING_DOWN)) {
        *dx = 0;
        *dy = -1;
        *gameStarted = 1;
    }
    else if ((ch == 'a' || ch == KEY_LEFT) && !(sb_get_size(sb) > 1 && GOING_RIGHT)) {
        *dx = -1;
        *dy = 0;
        *gameStarted = 1;
    }
    else if ((ch == 's' || ch == KEY_DOWN) && !(sb_get_size(sb) > 1 && GOING_UP)) {
        *dx = 0;
        *dy = 1;
        *gameStarted = 1;
    }
    else if ((ch == 'd' || ch == KEY_RIGHT) && !(sb_get_size(sb) > 1 && GOING_LEFT)) {
        *dx = 1;
        *dy = 0;
        *gameStarted = 1;
    }
}

void update_food_pos(coordinate_t *food_pos, int occupancy_grid[NUMROWS][NUMCOLS]) 
{
    do {
        food_pos->x = rand() % NUMCOLS;
        food_pos->y = rand() % NUMROWS;
    } while (is_snake(occupancy_grid, food_pos->x, food_pos->y));
}

int move_snake(snake_buffer_t *sb, coordinate_t *prev_tail_pos, int dx, int dy, int occupancy_grid[NUMROWS][NUMCOLS], coordinate_t *food_pos, int *score)
{
    coordinate_t head_pos;
    sb_peek_head(sb, &head_pos);
    coordinate_t next_head_pos = {.x=head_pos.x + dx, .y=head_pos.y + dy};

    if (!in_bounds(next_head_pos) || occupancy_grid[next_head_pos.y][next_head_pos.x] == 1) {
        return 0;
    }
    sb_put_head(sb, next_head_pos);
    occupancy_grid[next_head_pos.y][next_head_pos.x] = 1;

    if (food_is_touched(next_head_pos, *food_pos)) {
        (*score)++;
        if (*score < MAX_SCORE) {
            update_food_pos(food_pos, occupancy_grid);
        }
    }
    else {
        // Only shift tail if we don't eat
        sb_get_tail(sb, prev_tail_pos);
        occupancy_grid[prev_tail_pos->y][prev_tail_pos->x] = 0;
    }

    return 1;
}

int main(void)
{
    coordinate_t start_head_pos = {.x=NUMCOLS / 2, .y=NUMROWS / 2};
    snake_buffer_t sb = {.head=0, .tail=0};
    coordinate_t prev_tail_pos;
    sb_put_head(&sb, start_head_pos);

    int occupancy_grid[NUMROWS][NUMCOLS] = {0};
    occupancy_grid[start_head_pos.y][start_head_pos.x] = 1;  // 1 = occupied by snake

    srand(time(NULL));
    coordinate_t food_pos;
    update_food_pos(&food_pos, occupancy_grid);

    int dx = 0, dy = 0;
    int ch;
    int score = 0;
    int game_started = 0;

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);  // Make getch() non blocking. In no-delay mode, if no input is waiting, the value ERR is returned

    while (score < MAX_SCORE) {
        ch = getch();
        if (ch != ERR) {
            update_direction(&dx, &dy, ch, &sb, &game_started);
        }
        if (game_started && !move_snake(&sb, &prev_tail_pos, dx, dy, occupancy_grid, &food_pos, &score)) {
            break;
        }
        print_score(&score);
        print_board(occupancy_grid, food_pos);
        refresh();
        usleep(200000);
    }

    print_game_over(&score);
    refresh();

    timeout(-1);
    getch();
    endwin();

    return 0;
}