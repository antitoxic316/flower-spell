#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <MagickCore/MagickCore.h>

#include "image.h"
#include "ascii.h"

#define SPRITE_SIZE 16

#define GAMMA_COLOR_TO_LIN(colorChannel) \
  if ( colorChannel <= 0.04045 ) { \
    colorChannel /= 12.92; \
  } else { \
    colorChannel = pow((( colorChannel + 0.055)/1.055),2.4); \
  }


static double get_percieved_lightness(uint16_t r, uint16_t g, uint16_t b){
  double vR,vG,vB;
  double linR,linG,linB;
  double Y;

  vR = r / 65536.0;
  vG = g / 65536.0;
  vB = b / 65536.0;

  //printf("normalized colors: %f %f %f\n", vR, vG, vB);

  linR = vR; linG = vG; linB = vB;

  GAMMA_COLOR_TO_LIN(linR)
  GAMMA_COLOR_TO_LIN(linG)
  GAMMA_COLOR_TO_LIN(linB)

  Y = (0.2126 * linR + 0.7152 * linG + 0.0722 * linB);

  if ( Y <= (216.0/24389.0)) {       // The CIE standard states 0.008856 but 216/24389 is the intent for 0.008856451679036
    return Y * (24389.0/27.0);  // The CIE standard states 903.3, but 24389/27 is the intent, making 903.296296296296296
  } else {
    return pow(Y,(1.0/3.0)) * (double)(116 - 16);
  }
}


Image *img_generate_sprite_frames(char *source_filename)
{
  ExceptionInfo
    *exception;

  Image
    *image,
    *resized_images;

  ImageInfo
    *image_info;

  MagickCoreGenesis(source_filename,MagickTrue);
  exception=AcquireExceptionInfo();
  image_info=CloneImageInfo((ImageInfo *) NULL);
  
  (void) strcpy(image_info->filename, source_filename);
  
  image=ReadImage(image_info,exception);
  if (exception->severity != UndefinedException)
    CatchException(exception);
  if (image == (Image *) NULL)
    exit(1);
  
  int x = 0;

  resized_images = NewImageList();
  RectangleInfo rec;

  rec.y = 0;
  rec.width = SPRITE_SIZE;
  rec.height = SPRITE_SIZE;

  while(x+SPRITE_SIZE <= image->columns){
    rec.x = x;

    Image *cropped_img = CropImage(image, &rec, exception);

    cropped_img=ResizeImage(cropped_img, rec.width * 1.5, rec.height, BoxFilter,exception);
    if (cropped_img == (Image *) NULL)
      MagickError(exception->severity,exception->reason,exception->description);

    AppendImageToList(&resized_images, cropped_img);
    x += SPRITE_SIZE;
  }
  
// writing frames to seperate files 
/*
  int i = 0;
  Image *sprite;
  while((sprite = GetImageFromList(resized_images, i)) != (Image *) NULL){
    char buff[64] = {'\0', };
    snprintf(buff, 64, "%d.png", i);
    strcpy(sprite->filename, buff);
    WriteImage(image_info, sprite, exception);
    i++;
  }
*/
  
  
  image=DestroyImage(image);
  image_info=DestroyImageInfo(image_info);
  exception=DestroyExceptionInfo(exception);
  MagickCoreTerminus();

  return resized_images;
}

void img_transform_to_ascii(
  Image *img, 
  char ***char_map, 
  size_t *w, 
  size_t *h
){
  const Quantum *pixels_q;
  ExceptionInfo *ex;

  printf("img: %d %d\n", img->columns, img->rows);

  char **cm = ascii_map_init(img->columns, img->rows);

  pixels_q = GetVirtualPixels(img, 0, 0, img->columns, img->rows, ex);

  int pxl_c = 0;

  for(size_t y = 0; y < (size_t) img->rows; y++){
    for(size_t x = 0; x < (size_t) img->columns; x++){
      unsigned offset = GetPixelChannels(img) * (img->columns * y + x);

      uint16_t r = pixels_q[offset];
      uint16_t g = pixels_q[offset+1];
      uint16_t b = pixels_q[offset+2];

      double Y_st = get_percieved_lightness(r,g,b);


      int br = (int)(Y_st/10);
      cm[y][x] = ascii_br[br];
    }
  }

  *w = img->columns;
  *h = img->rows;
  *char_map = cm;
}