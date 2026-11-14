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

int in_bounds(int snake_pos_x, int snake_pos_y) {
    return snake_pos_x > -1 && snake_pos_x < NUMCOLS && snake_pos_y > -1 && snake_pos_y < NUMROWS;
}

int main(void)
{
    int snake_pos_x = 5, snake_pos_y = 5;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);       /* We get F1, F2, arrow keys etc..                           */

    while (in_bounds(snake_pos_x, snake_pos_y)) {
        move(0, 0);  // Reset the cursor to top left
        print_board(snake_pos_x, snake_pos_y);
        usleep(200000);
        snake_pos_x++;
    }

    getch();
    endwin();

    return 0;
}