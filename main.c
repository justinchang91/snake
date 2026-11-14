#include <stdio.h>
#include <unistd.h>

#define NUMROWS 10
#define NUMCOLS 10

void print_board(int snake_pos_x, int snake_pos_y)
{
    for (int r = 0; r < NUMROWS; r++) {
        for (int c = 0; c < NUMCOLS; c++) {
            if (r == snake_pos_y && c == snake_pos_x) {
                printf("@ ");
            }
            else {
                printf(". ");
            }
        }
        printf("\n");
    }
    fflush(stdout);
}

int in_bounds(int snake_pos_x, int snake_pos_y) {
    return snake_pos_x > -1 && snake_pos_x < NUMCOLS && snake_pos_y > -1 && snake_pos_y < NUMROWS;
}

int main(void)
{
    int snake_pos_x = 5, snake_pos_y = 5;

    while (in_bounds(snake_pos_x, snake_pos_y)) {
        printf("\033[H\033[2J");   /* move cursor to top-left, clear screen */
        print_board(snake_pos_x, snake_pos_y);
        usleep(200000);
        snake_pos_x++;
    }

    return 0;
}