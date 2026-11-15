#include "ring_buffer.h"

int sb_put_head(snake_buffer_t *sb, coordinate_t pos) 
{
    int next = (sb->head + 1) & (RB_SIZE-1);
    if (next == sb->tail) {
        return 0;
    }
    sb->buf[sb->head] = pos;
    sb->head = next;
    return 1;
}

int sb_get_tail(snake_buffer_t *sb, coordinate_t *prev_tail_pos, int *prev_tail_index) 
{
    int next = (sb->tail + 1) % RB_SIZE;
    if (sb->tail == sb->head) {
        return 0;
    }
    *prev_tail_pos = sb->buf[sb->tail];
    *prev_tail_index = sb->tail;
    sb->tail = next;
    return 1;
}

int sb_peek_head(const snake_buffer_t *sb, coordinate_t *pos) 
{
    int previous = sb->head == 0 ? RB_SIZE - 1 : sb->head - 1;
    if (sb->tail == sb->head) {
        return 0;
    }
    *pos = sb->buf[previous];
    return 1;
}

int sb_eat(snake_buffer_t *sb, coordinate_t prev_tail_pos, int prev_tail_index) 
{
    if (prev_tail_index == sb->head) {
        return 0;
    }
    sb->buf[prev_tail_index] = prev_tail_pos;
    sb->tail = prev_tail_index;
    return 1;
}

int sb_get_size(const snake_buffer_t *sb) {
    int count = 0;
    for (int i = sb->tail; i != sb->head; i = (i + 1) % RB_SIZE) {
        count++;
    }
    return count;
}