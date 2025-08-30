#pragma once

#include <stdint.h>
#include <ncurses.h>

extern char ascii_br[10];
extern int horizon_line;

char** ascii_map_init(uint32_t w, uint32_t h);
void ascii_map_print(char**, uint32_t w, uint32_t h);
void ascii_map_draw_on(
  char **arr1, size_t w1, size_t h1,
  char **arr2, size_t w2, size_t h2,
  int x, int y
);