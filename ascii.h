#pragma once

#include <stdint.h>
#include <ncurses.h>

#include "image.h"

extern char ascii_br[10];
extern int horizon_line;

struct char_img {
  uint32_t w;
  uint32_t h;
  int id;
  struct frame_list *fl;
  char **ch_m;
};

struct ascii_canvas {
  struct char_img *bg_image;
  struct char_img **images_list;
  int images_count;
};

struct char_img *char_img_new_from_file(char* filename);
void char_img_free(struct char_img *ch_i);

struct ascii_canvas *ascii_canvas_new(uint32_t w, uint32_t h);
void ascii_canvas_print(struct ascii_canvas *canv);
void ascii_canvas_draw_on(
  struct ascii_canvas *canv,
  struct char_img *img,
  int x, int y
);

struct char_img *char_img_new_from_pixels(Image *img);

void char_img_next_frame(struct char_img *ch_i);