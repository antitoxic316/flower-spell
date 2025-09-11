#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <MagickCore/MagickCore.h>

#include "image.h"

#define SPRITE_SIZE 16

#define GAMMA_COLOR_TO_LIN(colorChannel) \
  if ( colorChannel <= 0.04045 ) { \
    colorChannel /= 12.92; \
  } else { \
    colorChannel = pow((( colorChannel + 0.055)/1.055),2.4); \
  }


double get_percieved_lightness(uint16_t r, uint16_t g, uint16_t b){
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


struct frame_list *frame_list_new_from_file(char *source_filename)
{
  ExceptionInfo *exception;
  Image *image, *resized_images;
  ImageInfo *image_info;
  struct frame_list *fl;

  fl = malloc(sizeof(struct frame_list));
  fl->frame_count = 0;

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

    cropped_img=ResizeImage(cropped_img, (float)rec.width * 1.8, rec.height, BoxFilter,exception);
    if (cropped_img == (Image *) NULL){
      MagickError(exception->severity,exception->reason,exception->description);
    }

    AppendImageToList(&resized_images, cropped_img);
    x += SPRITE_SIZE;

    fl->frame_count++;
  }

  fl->frames = resized_images;
  fl->frame_i = 0;
  
  image=DestroyImage(image);
  image_info=DestroyImageInfo(image_info);
  exception=DestroyExceptionInfo(exception);
  MagickCoreTerminus();

  return fl;
}

Image *frame_list_turn_frame(struct frame_list *fl){
  if(fl->frame_i >= fl->frame_count){
    return NULL;
  }

  Image *frame;

  frame = GetImageFromList(fl->frames, fl->frame_i);
  if(frame == NULL){
      printf("failed to parse frame\n");
      return NULL;
  }

  fl->frame_i++;

  return frame;
}