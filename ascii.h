#pragma once

#include <stdint.h>
#include <ncurses.h>
#include <stdbool.h>

#include "image.h"

extern char ascii_br[10];
extern int horizon_line;
extern int view_plane_z;

struct char_gif {
  uint32_t w;
  uint32_t h;
  uint32_t wP; // percieved width
  uint32_t hP;  // percieved height
  int id;
  struct frame_list *fl;
  char **ch_m;
  size_t x; //view plane
  size_t y; //view plane
  int xR; //real x
  int yR; //real y
	int zR; //real z
	bool resized;
};

struct ascii_canvas {
  struct char_gif *bg_image;
  struct char_gif **images_list;
  int images_count;
  size_t viewerX;
};

struct char_gif *char_img_new_from_file(char* filename);
void char_img_free(struct char_gif *ch_i);

struct ascii_canvas *ascii_canvas_new(uint32_t w, uint32_t h);
void ascii_canvas_print(struct ascii_canvas *canv);

//x y and z are real world coords
void ascii_canvas_draw_on(
  struct ascii_canvas *canv,
  struct char_gif *img
);



void char_img_next_frame(struct char_gif *ch_i);

void ascii_canvas_progress_scene(struct ascii_canvas* canv);

void ascii_canvas_add_img(struct ascii_canvas *canv, struct char_gif *img);

void char_gif_move(struct ascii_canvas* canv, struct char_gif *chg, int x, int y);
