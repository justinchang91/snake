#include <ncurses.h>
#include <unistd.h>
#include "ring_buffer.h"

#define NUMROWS 10
#define NUMCOLS 10

int is_snake(snake_buffer_t *sb, int x, int y) 
{
    for (int i = sb->tail; i != sb->head; i = (i + 1) % RB_SIZE) {
        if (sb->buf[i].x == x && sb->buf[i].y == y) {
            return 1;
        }
    }
    return 0;
}

int is_food(coordinate_t food_pos, int x, int y)
{
    return (y == food_pos.y && x == food_pos.x);
}

void print_board(snake_buffer_t *sb, coordinate_t food_pos)
{
    for (int r = 0; r < NUMROWS; r++) {
        for (int c = 0; c < NUMCOLS; c++) {
            if (is_snake(sb, c, r)) {
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
    refresh();
}

int in_bounds(const coordinate_t snake_head_pos) 
{
    return snake_head_pos.x > -1 && snake_head_pos.x < NUMCOLS && snake_head_pos.y > -1 && snake_head_pos.y < NUMROWS;
}

int food_is_touched(const coordinate_t snake_head_pos, const coordinate_t food_pos)
{
    return snake_head_pos.x == food_pos.x && snake_head_pos.y == food_pos.y;
}

void update_direction(int *dx, int *dy, int ch) 
{
    switch (ch) {
        case 'w':
        case KEY_UP:
            *dx = 0;
            *dy = -1;
            break;
        case 'a':
        case KEY_LEFT:
            *dx = -1;
            *dy = 0;
            break;
        case 's':
        case KEY_DOWN:
            *dx = 0;
            *dy = 1;
            break;
        case 'd':
        case KEY_RIGHT:
            *dx = 1;
            *dy = 0;
            break;
        default:
            break;
    }
}

void move_snake(snake_buffer_t *sb, int dx, int dy)
{
    // Use the ring buffer
    // When snake moves, it's a put for the new head coordinate, followed by a get for the tail
    coordinate_t head_pos;
    sb_peek_head(sb, &head_pos);
    coordinate_t next_head_pos = {.x=head_pos.x + dx, .y=head_pos.y + dy};
    sb_put(sb, next_head_pos);
    coordinate_t tail_pos;
    sb_get(sb, &tail_pos);
}

int main(void)
{
    coordinate_t start_head_pos = {.x=5, .y=5};
    snake_buffer_t sb = {.head=0, .tail=0};
    sb_put(&sb, start_head_pos);

    coordinate_t food_pos = {.x=8, .y=8};
    int dx = 0, dy = 0;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);  // Make getch() non blocking. In no-delay mode, if no input is waiting, the value ERR is returned

    while (1) {
        ch = getch();
        if (ch != ERR) {
            update_direction(&dx, &dy, ch);
        }
        move_snake(&sb, dx, dy);
        coordinate_t snake_head_pos;
        sb_peek_head(&sb, &snake_head_pos);
        if (!in_bounds(snake_head_pos)) {
            break;
        }
        if (food_is_touched(snake_head_pos, food_pos)) {
            sb_put(&sb, snake_head_pos);
        }
        move(0, 0);  // Reset the cursor to top left
        print_board(&sb, food_pos);
        usleep(200000);
    }

    mvprintw(NUMROWS + 2, 0, "Game over! Press any key to exit.");
    refresh();

    timeout(-1);
    getch();
    endwin();

    return 0;
}