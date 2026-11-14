#include <ncurses.h>
#include <unistd.h>

#define NUMROWS 10
#define NUMCOLS 10

void print_board(int snake_pos_x, int snake_pos_y)
{
    for (int r = 0; r < NUMROWS; r++) {
        for (int c = 0; c < NUMCOLS; c++) {
            if (r == snake_pos_y && c == snake_pos_x) {
                printw("@ ");
            }
            else {
                printw(". ");
            }
        }
        printw("\n");
    }
    refresh();
}

int in_bounds(int snake_pos_x, int snake_pos_y) 
{
    return snake_pos_x > -1 && snake_pos_x < NUMCOLS && snake_pos_y > -1 && snake_pos_y < NUMROWS;
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

int main(void)
{
    int snake_pos_x = 5, snake_pos_y = 5;
    int dx = 0, dy = 0;
    int ch;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);  // Make getch() non blocking. In no-delay mode, if no input is waiting, the value ERR is returned

    while (in_bounds(snake_pos_x, snake_pos_y)) {
        ch = getch();
        if (ch != ERR) {
            update_direction(&dx, &dy, ch);
        }
        move(0, 0);  // Reset the cursor to top left
        print_board(snake_pos_x, snake_pos_y);
        usleep(200000);
        snake_pos_x += dx;
        snake_pos_y += dy;
    }

    mvprintw(NUMROWS + 2, 0, "Game over! Press any key to exit.");
    refresh();

    timeout(-1);
    getch();
    endwin();

    return 0;
}