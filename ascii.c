#include "ascii.h"

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char ascii_br[10] = {' ','.',':','-','=','+','*','#','%','@'};
int horizon_line = 20;

int ascii_canvas_get_brightness_lv(char ch){
  int br = -1;
  for(int ii = br; ii < 10; ii++){
    if(ch == ascii_br[ii]){
      br = ii;
      break;
    }
  }

  return br;
}

struct char_gif *canvas_background_new(size_t w, size_t h){
  struct char_gif *ch_img = malloc(sizeof(struct char_gif));
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

  ch_img->ch_m = char_map;
  ch_img->w = w;
  ch_img->h = h;
  ch_img->id = 0;

  return ch_img;
}

struct ascii_canvas *ascii_canvas_new(uint32_t width, uint32_t height){
  struct ascii_canvas *canv = malloc(sizeof (struct ascii_canvas));

  canv->bg_image = canvas_background_new(width, height);
  canv->images_list = NULL;
  canv->images_count = 0;

  return canv;
}

void ascii_canvas_draw_on(struct ascii_canvas *canv, struct char_gif *img, int x, int y)
{
  if(canv->bg_image->w < img->w || canv->bg_image->h < img->h){
    printf("ascii_canvas_draw_on error: img->ch_m should be smaller then canv->ch_m");
    return;
  }
  if(x >= canv->bg_image->w || y >= canv->bg_image->h){
    printf("ascii_canvas_draw_on error: out of bounds draw");
  }
  
  for(int i = 0; i < img->h; i++){
    if(i+y >= canv->bg_image->h){
      break;
    }
    for(int j = 0; j < img->w; j++){
      if(j+x >= canv->bg_image->w){
        break;
      }


      // Don't override brightness level
      // so background can be saved
      int br_bg = ascii_canvas_get_brightness_lv(canv->bg_image->ch_m[i+y][j+x]);
      int br_img = ascii_canvas_get_brightness_lv(img->ch_m[i][j]);
      if(br_bg > br_img){
        continue;
      }

      canv->bg_image->ch_m[i+y][j+x] = img->ch_m[i][j];
    }
  }
}

void ascii_canvas_add_img(struct ascii_canvas *canv, struct char_gif *img, size_t x, size_t y){
  canv->images_count++;
  struct char_gif **new_img_l;
  new_img_l = realloc(canv->images_list, canv->images_count*sizeof(struct char_gif*));
  if(!new_img_l){
    printf("realloc: failed to allocate memory");
    return;
  }
  canv->images_list = new_img_l;
  canv->images_list[canv->images_count-1] = img;
  img->x = x;
  img->y = y;
}

void ascii_canvas_progress_scene(struct ascii_canvas* canv){
  for(int i = 0 ; i < canv->images_count; i++){
      struct char_gif *ch_i = canv->images_list[i];
      char_img_next_frame(ch_i);
      ascii_canvas_draw_on(canv, ch_i, ch_i->x, ch_i->y); 
  }
}

void ascii_canvas_print(struct ascii_canvas *canv){
    //bitmap cropping to termianal size
    uint32_t w_drw_lim = canv->bg_image->w;
    uint32_t h_drw_lim = canv->bg_image->h;
    if(canv->bg_image->w > COLS){
        w_drw_lim = COLS;
    }
    if(canv->bg_image->h > LINES){
        h_drw_lim = LINES;
    }

    for(int i = 0; i < h_drw_lim; i++){
        for(int j = 0; j < w_drw_lim; j++){
            printw("%c", canv->bg_image->ch_m[i][j]);
        }
    }

    for(int i = 0; i < canv->images_count; i++){
      struct char_gif *ch_i = canv->images_list[i];
      ascii_canvas_draw_on(canv, ch_i, ch_i->x, ch_i->y); 
    }
} 

void ascii_canvas_free(struct ascii_canvas *canv){
  for(int i = 0; i < canv->images_count; i++){
    char_img_free(canv->images_list[i]);
  }
  char_img_free(canv->bg_image);

  free(canv);
}

//helper function for frame switching
//char map will be the same size as img dimensions
char **char_map_new_from_image(Image *img){
  const Quantum *pixels_q;
  ExceptionInfo *ex;

  ex = AcquireExceptionInfo();

  char **ch_m = malloc(sizeof(char*) * img->rows);

  for(int i = 0; i < img->rows; i++){
      char *initiated_line = calloc(img->columns, sizeof(char));
      memset(initiated_line, ascii_br[0], img->columns);
      ch_m[i] = initiated_line;
  }

  pixels_q = GetVirtualPixels(img, 0, 0, img->columns, img->rows, ex);

  for(size_t y = 0; y < (size_t) img->rows; y++){
    for(size_t x = 0; x < (size_t) img->columns; x++){
      unsigned offset = GetPixelChannels(img) * (img->columns * y + x);

      int br = get_brightness_lv(pixels_q, offset, 10);
      ch_m[y][x] = ascii_br[br];
    }
  }

  return ch_m;
}

struct char_gif *char_img_new_from_file(char *filename){
  struct char_gif *ch_img = malloc(sizeof(struct char_gif));

  ch_img->fl = frame_list_new_from_file(filename);
  Image *first_frame = frame_list_turn_frame(ch_img->fl);
  ch_img->fl->frame_i--;

  ch_img->ch_m = char_map_new_from_image(first_frame);
  ch_img->w = first_frame->columns;
  ch_img->h = first_frame->rows;
  ch_img->id = (int)100*rand();
  
  return ch_img;
}

void char_img_free(struct char_gif* ch_i){
  for(int i = 0; i < ch_i->h; i++){
    free(ch_i->ch_m[i]);
  }
  free(ch_i->ch_m);

  //frame_list_free(ch_i->fl);

  free(ch_i);
}

void char_img_next_frame(struct char_gif *ch_i){
  Image *next_frame = frame_list_turn_frame(ch_i->fl);
  
  if(!next_frame){
    return;
  }

  for(int i = 0; i < ch_i->h; i++){
    free(ch_i->ch_m[i]);
  }
  free(ch_i->ch_m);

  char **new_ch_m = char_map_new_from_image(next_frame);

  ch_i->ch_m = new_ch_m;
}