#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <MagickCore/MagickCore.h>

#include "image.h"

#define ASSIGN_MAX(a, b) if(a < b){ a = b; }
#define ASSIGN_MIN(a, b) if(a > b){ a = b; } 

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

int get_brightness_lv(const Quantum *pixels,unsigned offset, int max_lv){
  uint16_t r = pixels[offset];
  uint16_t g = pixels[offset+1];
  uint16_t b = pixels[offset+2];

  double Y_st = get_percieved_lightness(r,g,b);
  int br = (int)(Y_st/max_lv);

  return br;
}

//crops all the sprites to the box where all the rows and columns are used by the largest sprite
void frame_list_cut_empty_space(struct frame_list *fl){
  ExceptionInfo *ex;
  int fl_st_u = INT32_MAX, fl_st_d = 0;
  int fl_st_l = INT32_MAX, fl_st_r = 0;

  ex = AcquireExceptionInfo();

  //calculate largest needed box
  for(int i = 0; i < fl->frame_count; i++) {
    Image *img = GetImageFromList(fl->frames, i);
    if (img == (Image *) NULL){
      MagickError(ex->severity,ex->reason,ex->description);
    }
    bool met_sprite = false;
    bool ended_sprite = false;
    int spr_st_u = 0, spr_st_d = 0;
    
    for(int y = 0; y < img->rows; y++){
      const Quantum *col_pixels;
      col_pixels = GetVirtualPixels(img, 0, y, img->columns, y+1, ex);
      if (col_pixels == (Quantum *) NULL){
        MagickError(ex->severity,ex->reason,ex->description);
      }
      bool all_dark = true;

      for(int x = 0; x < img->columns; x++){
        unsigned offset = GetPixelChannels(img) * x;

        int br = get_brightness_lv(col_pixels, offset, 10);
        
        if(br > 0){
          all_dark = false;
        }
      } // x loop end
      if(!all_dark && !met_sprite){
        spr_st_u = y;
        met_sprite = true;
      }
      if(!all_dark && met_sprite && ended_sprite){
        ended_sprite = false;
      }
      if(all_dark && met_sprite && !ended_sprite){
        spr_st_d = y;
        ended_sprite = true;
      }
    } // y loop end
    if(!ended_sprite){
      spr_st_d = img->rows-1;
    }
    ASSIGN_MAX(fl_st_d, spr_st_d)
    ASSIGN_MIN(fl_st_u, spr_st_u)
  } // sprite loop end


  for(int i = 0; i < fl->frame_count; i++) {
    Image *img = GetImageFromList(fl->frames, i);
    if (img == (Image *) NULL){
      MagickError(ex->severity,ex->reason,ex->description);
    }
    bool met_sprite = false;
    bool ended_sprite = false;
    int spr_st_l = 0, spr_st_r = 0;
    
    for(int x = 0; x < img->columns; x++){
      const Quantum *row_pixels;
      row_pixels = GetVirtualPixels(img, x, 0, x+1, img->rows, ex);
      if (row_pixels == (Quantum *) NULL){
        MagickError(ex->severity,ex->reason,ex->description);
      }
      bool all_dark = true;

      for(int y = 0; y < img->rows; y++){
        unsigned offset = GetPixelChannels(img) * (img->columns * y + x);

        int br = get_brightness_lv(row_pixels, offset, 10);
        
        if(br > 0){
          all_dark = false;
        }
      } // y loop end
      if(!all_dark && !met_sprite){
        spr_st_l = x;
        met_sprite = true;
      }
      if(!all_dark && met_sprite && ended_sprite){
        ended_sprite = false;
      }
      if(all_dark && met_sprite && !ended_sprite){
        spr_st_r = x;
        ended_sprite = true;
      }
    } // x loop end
    if(!ended_sprite){
      spr_st_r = img->rows-1;
    }
    ASSIGN_MAX(fl_st_r, spr_st_r)
    ASSIGN_MIN(fl_st_l, spr_st_l)
  } // sprite loop end

  //crop all the sprites
  RectangleInfo rec;
  rec.x = fl_st_l;
  rec.y = fl_st_u;
  rec.width = fl_st_r-fl_st_l;
  rec.height = fl_st_d-fl_st_u;

  Image *new_image_list = NewImageList();

  for(int i = 0; i < fl->frame_count; i++){
    Image *img = GetImageFromList(fl->frames, i);

    //because CropImage changes page value
    img->page.x = 0;
    img->page.y = 0;
    img->page.width = 0;
    img->page.height = 0;
    Image *cropped_img = CropImage(img, &rec, ex);

    if (!cropped_img){
      MagickError(ex->severity,ex->reason,ex->description);
    }
    AppendImageToList(&new_image_list, cropped_img);
  }

  DestroyImageList(fl->frames);
  fl->frames = new_image_list;
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

    cropped_img=ResizeImage(cropped_img, (float)rec.width * FONT_PROPOTION_BIAS, rec.height, BoxFilter, exception);
    if (cropped_img == (Image *) NULL){
      MagickError(exception->severity,exception->reason,exception->description);
    }
    AppendImageToList(&resized_images, cropped_img);
    x += SPRITE_SIZE;

    fl->frame_count++;
  }

  fl->frames = resized_images;
  fl->frame_i = 0;

  frame_list_cut_empty_space(fl);
  
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