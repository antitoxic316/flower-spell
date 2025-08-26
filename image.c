#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MagickCore/MagickCore.h>

int main(int argc,char **argv)
{
  ExceptionInfo
    *exception;

  Image
    *image,
    *resize_image;

  ImageInfo
    *image_info;

  /*
    Initialize the image info structure and read an image.
  */
  MagickCoreGenesis(*argv,MagickTrue);
  exception=AcquireExceptionInfo();
  image_info=CloneImageInfo((ImageInfo *) NULL);
  
  (void) strcpy(image_info->filename, "untitled.png");
  
  image=ReadImage(image_info,exception);
  if (exception->severity != UndefinedException)
    CatchException(exception);
  if (image == (Image *) NULL)
    exit(1);
  
  /*
    Convert the image to a thumbnail.
  */
    resize_image=ResizeImage(image,41,78,LanczosFilter,exception);
    if (resize_image == (Image *) NULL)
      MagickError(exception->severity,exception->reason,exception->description);
    DestroyImage(image);
  
  /*
    Write the image thumbnail.
  */
  (void) strcpy(resize_image->filename, "resized.png");
  WriteImage(image_info, resize_image, exception);
  
  /*
    Destroy the image thumbnail and exit.
  */
  resize_image=DestroyImage(resize_image);
  image_info=DestroyImageInfo(image_info);
  exception=DestroyExceptionInfo(exception);
  MagickCoreTerminus();
  return(0);
}