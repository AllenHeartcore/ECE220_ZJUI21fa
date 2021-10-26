#include "lodepng.h"
#include "imageData.h"
#include "lab7.h"

//function for encoding a PNG
void encode(const char *filename,Image *encodeImage)
{
  int width=encodeImage->width;
  int height=encodeImage->height;
  unsigned char *image=(unsigned char*)malloc(sizeof(unsigned char)*4*height*width);
  
  int row,col;
  int imageWidth=4*width;
  uint8_t *redChannel=encodeImage->redChannel;
  uint8_t *greenChannel=encodeImage->greenChannel;
  uint8_t *blueChannel=encodeImage->blueChannel;
  uint8_t *alphaChannel=encodeImage->alphaChannel;

  for(row=0;row<height;row++)
    for(col=0;col<width;col++)
    {
      image[imageWidth*row+4*col+0]=redChannel[row*width+col];
      image[imageWidth*row+4*col+1]=greenChannel[row*width+col];
      image[imageWidth*row+4*col+2]=blueChannel[row*width+col];
      image[imageWidth*row+4*col+3]=alphaChannel[row*width+col];
    }

  unsigned int error=lodepng_encode32_file(filename,image,width,height);
  if(error)
    printf("error %u:%s\n",error,lodepng_error_text(error));

  free(image);
}

//function for decoding a PNG
Image *decode(const char *filename)
{
  Image *inputImage=(Image*)malloc(sizeof(Image));
  unsigned int error;
  unsigned char *image;

  //FIX: fix this part here
  error=lodepng_decode32_file(&image,(unsigned int*)&(inputImage->width),(unsigned int*)&(inputImage->height),filename);
  if(error)
    printf("error %u: %s\n",error,lodepng_error_text(error));

  int width=inputImage->width;
  int height=inputImage->height;
  inputImage->redChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  inputImage->greenChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  inputImage->blueChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  inputImage->alphaChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);

  int row,col;
  int imageWidth=width*4;
  uint8_t *redChannel=inputImage->redChannel;
  uint8_t *greenChannel=inputImage->greenChannel;
  uint8_t *blueChannel=inputImage->blueChannel;
  uint8_t *alphaChannel=inputImage->alphaChannel;

  for(row=0;row<height;row++)
  {
    for(col=0;col<width;col++)
    {
      redChannel[row*width+col]=image[imageWidth*row+4*col+0];
      greenChannel[row*width+col]=image[imageWidth*row+4*col+1];
      blueChannel[row*width+col]=image[imageWidth*row+4*col+2];
      alphaChannel[row*width+col]=image[imageWidth*row+4*col+3];
    }
  }

  free(image);
  return inputImage;
}

Image *generateOutput(Image *inputImage)
{
  Image *outputImage=(Image*)malloc(sizeof(Image));
  int width=inputImage->width;
  int height=inputImage->height;
  outputImage->width=width;
  outputImage->height=height;
  outputImage->redChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  outputImage->greenChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  outputImage->blueChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  outputImage->alphaChannel=(uint8_t*)malloc(sizeof(uint8_t)*width*height);
  return outputImage;
}

void freeImage(Image *inputImage)
{
  free(inputImage->redChannel);
  free(inputImage->greenChannel);
  free(inputImage->blueChannel);
  free(inputImage->alphaChannel);
  free(inputImage);
}
