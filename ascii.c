#include "ascii.h"

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char ascii_br[10] = {' ','.',':','-','=','+','*','#','%','@'};
int horizon_line = 20;

int ascii_map_get_brightness_lv(char ch){
  int br = -1;
  for(int ii = br; ii < 10; ii++){
    if(ch == ascii_br[ii]){
      br = ii;
      break;
    }
  }

  return br;
}

char** ascii_map_init(uint32_t w, uint32_t h){
    char **char_map = malloc(h * sizeof(char*));
    
    for(int i = 0; i < h; i++){
        char *initiated_line = calloc(w, sizeof(char));
        if(i > horizon_line){
          memset(initiated_line, ascii_br[1], w);
        } else {
          memset(initiated_line, ascii_br[0], w);
        }
        char_map[i] = initiated_line;
    }

    return char_map;
}

void ascii_map_draw_on(
  char **arr1, size_t w1, size_t h1,
  char **arr2, size_t w2, size_t h2,
  int x, int y
)
{
  if(w1 < w2 || h1 < h2){
    printf("ascii_map_draw_on error: arr2 should be smaller then arr1");
    return;
  }
  if(x >= w1 || y >= h1){
    printf("ascii_map_draw_on error: out of bounds draw");
  }
  
  for(int i = 0; i < h2; i++){
    if(i+y >= h1){
      break;
    }
    for(int j = 0; j < w2; j++){
      if(j+x >= w1){
        break;
      }


      // Don't override brightness level
      // so background can be saved
      int br_bg = ascii_map_get_brightness_lv(arr1[i+y][j+x]);
      int br_img = ascii_map_get_brightness_lv(arr2[i][j]);

      if(br_bg > br_img){
        continue;
      }

      arr1[i+y][j+x] = arr2[i][j];
    }
  }
}

void ascii_map_print(char** char_map, uint32_t w, uint32_t h){
    //bitmap cropping to termianal size
    uint32_t w_drw_lim = w;
    uint32_t h_drw_lim = h;
    if(w > COLS){
        w_drw_lim = COLS;
    }
    if(h > LINES){
        h_drw_lim = LINES;
    }

    for(int i = 0; i < h_drw_lim; i++){
        for(int j = 0; j < w_drw_lim; j++){
            printw("%c", char_map[i][j]);
        }
    }
} 