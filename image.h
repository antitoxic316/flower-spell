#pragma once

#include <MagickCore/MagickCore.h>

struct frame_list {
  Image *frames;
  int frame_i;
  int frame_count;
};

Image *frame_list_turn_frame(struct frame_list *fl);

struct frame_list *frame_list_new_from_file(char *source_filename);

double get_percieved_lightness(uint16_t r, uint16_t g, uint16_t b);
