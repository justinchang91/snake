#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "ring_buffer.h"
#include "game.h"
#include <string.h>

_Static_assert(NUMROWS * NUMCOLS < RB_SIZE, "RB_SIZE too small for board");

static int going_down(int dx, int dy)  { return dx == 0 && dy == 1; }
static int going_up(int dx, int dy)    { return dx == 0 && dy == -1; }
static int going_left(int dx, int dy)  { return dx == -1 && dy == 0; }
static int going_right(int dx, int dy) { return dx == 1 && dy == 0; }

void update_food_pos(coordinate_t *food_pos, available_cells_t *available_cells) 
{
    int chosen_index = rand() % available_cells->size;
    food_pos->x = available_cells->buf[chosen_index].x;
    food_pos->y = available_cells->buf[chosen_index].y;
}

void init_available_cells(available_cells_t *available_cells, int occupancy_grid[NUMROWS][NUMCOLS]) 
{
    available_cells->size = 0;
    for (int r = 0; r < NUMROWS; r++) {
        for (int c = 0; c < NUMCOLS; c++) {
            if (occupancy_grid[r][c] == OCCUPIED) {
                continue;
            }
            coordinate_t coordinate = {.x=c, .y=r};
            available_cells->buf[available_cells->size] = coordinate;
            occupancy_grid[r][c] = available_cells->size;
            available_cells->size++;
        }
    }
}

void init_game(game_t *game) 
{
    coordinate_t start_head_pos = {.x=NUMCOLS / 2, .y=NUMROWS / 2};
    game->sb.head = 0;
    game->sb.tail = 0;
    sb_put_head(&game->sb, start_head_pos);
    memset(game->occupancy_grid, 0, sizeof(game->occupancy_grid));
    game->occupancy_grid[start_head_pos.y][start_head_pos.x] = OCCUPIED;
    init_available_cells(&game->available_cells, game->occupancy_grid);
    update_food_pos(&game->food_pos, &game->available_cells);
    game->direction.x = 0;
    game->direction.y = 0;
    game->score = 0;
    game->game_started = 0;
}

void init_screen() 
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);  // Make getch() non blocking. In no-delay mode, if no input is waiting, the value ERR is returned
}

void make_cell_not_available(available_cells_t *available_cells, int occupancy_grid[NUMROWS][NUMCOLS], coordinate_t position) {
    int index_in_available_cells = occupancy_grid[position.y][position.x];
    coordinate_t available_cells_tail = available_cells->buf[available_cells->size - 1];
    available_cells->buf[index_in_available_cells] = available_cells_tail;  
    occupancy_grid[available_cells_tail.y][available_cells_tail.x] = index_in_available_cells;
    available_cells->size--;
    occupancy_grid[position.y][position.x] = OCCUPIED; // Cell is no longer available
}

void make_cell_available(available_cells_t *available_cells, int occupancy_grid[NUMROWS][NUMCOLS], coordinate_t position) {
    available_cells->buf[available_cells->size] = position;
    available_cells->size++;
    occupancy_grid[position.y][position.x] = available_cells->size - 1;
}

int is_snake(int occupancy_grid[NUMROWS][NUMCOLS], const int x, const int y) 
{
    return occupancy_grid[y][x] == OCCUPIED;
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

void print_score(int score) {
    mvprintw(NUMROWS + 1, 0, "Score: %d", score);
}

void print_game_over(int score) {
    if (score == MAX_SCORE) {
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

void update_direction(coordinate_t *direction, const int ch, const snake_buffer_t *sb, int *game_started) 
{
    if ((ch == 'w' || ch == KEY_UP) && !(sb_get_size(sb) > 1 && going_down(direction->x, direction->y))) {
        direction->x = 0;
        direction->y = -1;
        *game_started = 1;
    }
    else if ((ch == 'a' || ch == KEY_LEFT) && !(sb_get_size(sb) > 1 && going_right(direction->x, direction->y))) {
        direction->x = -1;
        direction->y = 0;
        *game_started = 1;
    }
    else if ((ch == 's' || ch == KEY_DOWN) && !(sb_get_size(sb) > 1 && going_up(direction->x, direction->y))) {
        direction->x = 0;
        direction->y = 1;
        *game_started = 1;
    }
    else if ((ch == 'd' || ch == KEY_RIGHT) && !(sb_get_size(sb) > 1 && going_left(direction->x, direction->y))) {
        direction->x = 1;
        direction->y = 0;
        *game_started = 1;
    }
}

int move_snake(snake_buffer_t *sb, coordinate_t direction, int occupancy_grid[NUMROWS][NUMCOLS], coordinate_t *food_pos, int *score, available_cells_t *available_cells)
{
    coordinate_t head_pos;
    sb_peek_head(sb, &head_pos);
    coordinate_t next_head_pos = {.x=head_pos.x + direction.x, .y=head_pos.y + direction.y};

    if (!in_bounds(next_head_pos) || occupancy_grid[next_head_pos.y][next_head_pos.x] == OCCUPIED) {
        return 0;
    }
    sb_put_head(sb, next_head_pos);
    make_cell_not_available(available_cells, occupancy_grid, next_head_pos);

    if (food_is_touched(next_head_pos, *food_pos)) {
        (*score)++;
        if (*score < MAX_SCORE) {
            update_food_pos(food_pos, available_cells);
        }
    }
    else {
        // Only shift tail if we don't eat
        coordinate_t prev_tail_pos;
        sb_get_tail(sb, &prev_tail_pos);
        make_cell_available(available_cells, occupancy_grid, prev_tail_pos);
    }

    return 1;
}

void run_game(game_t *game)
{
    while (game->score < MAX_SCORE) {
        int ch = getch();
        if (ch != ERR) {
            update_direction(&game->direction, ch, &game->sb, &game->game_started);
        }
        if (game->game_started && !move_snake(&game->sb, game->direction, game->occupancy_grid, &game->food_pos, &game->score, &game->available_cells)) {
            break;
        }
        print_score(game->score);
        print_board(game->occupancy_grid, game->food_pos);
        refresh();
        usleep(200000);
    }
    print_game_over(game->score);
    refresh();
    timeout(-1);
    getch();
}

void end_screen() 
{
    endwin();
}

int main(void)
{
    srand(time(NULL));
    init_screen();

    game_t game;
    init_game(&game);
    run_game(&game);

    end_screen();
    return 0;
}