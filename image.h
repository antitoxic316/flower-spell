#pragma once

#include <MagickCore/MagickCore.h>

void img_transform_to_ascii(
  Image *img, 
  char ***char_map, 
  size_t *w, 
  size_t *h
);

Image *img_generate_sprite_frames(char *source_filename);