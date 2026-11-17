/* Rules 
- Empty: H == T
- Full: Next H position == T
- Put: place at H, shift H
- Get: take at T, shfit T (if T < H)
- Shift: index = (index + 1) % SIZE or index = (index + 1) & (SIZE - 1) 
*/
#define RB_SIZE 128

/* Storing coordinates X, Y*/
typedef struct {
    int x;
    int y;
} coordinate_t;

typedef struct {
    coordinate_t buf[RB_SIZE];
    int head;  // no need for volatile. No ISR reading it
    int tail;
} snake_buffer_t;

int sb_put_head(snake_buffer_t *sb, coordinate_t pos);
int sb_get_tail(snake_buffer_t *sb, coordinate_t *prev_tail_pos) ;
int sb_peek_head(const snake_buffer_t *sb, coordinate_t *pos);
int sb_get_size(const snake_buffer_t *sb);