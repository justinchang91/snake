# gdb helpers for the snake game.
# Load with:  (gdb) source grid.gdb
#
# pgrid - draw occupancy_grid like the game does (@ = snake, * = food, . = empty).
#         Must be run from main's frame (use `up` if stopped inside another function).
#         Keep the 10s in sync with NUMROWS / NUMCOLS in main.c.

define pgrid
  set $r = 0
  while $r < 10
    set $c = 0
    while $c < 10
      if occupancy_grid[$r][$c] == 1
        printf "@ "
      else
        if $r == food_pos.y && $c == food_pos.x
          printf "* "
        else
          printf ". "
        end
      end
      set $c = $c + 1
    end
    printf "\n"
    set $r = $r + 1
  end
end

document pgrid
Draw occupancy_grid as the game board (@ = snake, * = food, . = empty).
Run from main's frame.
end

# pocc - print the raw occupancy_grid values (0/1) as a grid.
#        Works in any frame that can see occupancy_grid (main, move_snake, etc.).

define pocc
  set $r = 0
  while $r < 10
    set $c = 0
    while $c < 10
      printf "%d ", occupancy_grid[$r][$c]
      set $c = $c + 1
    end
    printf "\n"
    set $r = $r + 1
  end
end

document pocc
Print the raw occupancy_grid values (0/1) as a 10x10 grid.
end
