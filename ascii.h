#pragma once

#include <stdint.h>
#include <ncurses.h>

#include "image.h"

extern char ascii_br[10];
extern int horizon_line;

struct char_gif {
  uint32_t w;
  uint32_t h;
  int id;
  struct frame_list *fl;
  char **ch_m;
  size_t x;
  size_t y;
};

struct ascii_canvas {
  struct char_gif *bg_image;
  struct char_gif **images_list;
  int images_count;
};

struct char_gif *char_img_new_from_file(char* filename);
void char_img_free(struct char_gif *ch_i);

struct ascii_canvas *ascii_canvas_new(uint32_t w, uint32_t h);
void ascii_canvas_print(struct ascii_canvas *canv);
void ascii_canvas_draw_on(
  struct ascii_canvas *canv,
  struct char_gif *img,
  int x, int y
);

void char_img_next_frame(struct char_gif *ch_i);

void ascii_canvas_progress_scene(struct ascii_canvas* canv);

void ascii_canvas_add_img(struct ascii_canvas *canv, struct char_gif *img, size_t x, size_t y);