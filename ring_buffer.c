#include "ring_buffer.h"

int sb_put_head(snake_buffer_t *sb, coordinate_t pos) 
{
    int next = (sb->head + 1) % RB_SIZE;
    if (next == sb->tail) {
        return 0;
    }
    sb->buf[sb->head] = pos;
    sb->head = next;
    return 1;
}

int sb_get_tail(snake_buffer_t *sb, coordinate_t *prev_tail_pos) 
{
    int next = (sb->tail + 1) % RB_SIZE;
    if (sb->tail == sb->head) {
        return 0;
    }
    *prev_tail_pos = sb->buf[sb->tail];
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

int sb_get_size(const snake_buffer_t *sb) {
    return (sb->head - sb->tail + RB_SIZE) % RB_SIZE;
}

// Two cases. In both cases, we try to calculate the distance from tail to head:
// 1. H > T -> head - tail
// 2. H < T -> RB_SIZE - tail + head -> rearrage to head - tail + RB_SIZE -> this result must be less than RB_SIZE
// To combine these two cases, do (head - tail + RB_SIZE) % RB_SIZE
// For case 1, you basically do + RB_SIZE - RB_SIZE to get back to head - tail
// For case 2, since head - tail + RB_SIZE is less than RB_SIZE, the % RB_SIZE does nothing