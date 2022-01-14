#include "ImageProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

ImageProcessing::ImageProcessing(
                                 const char *_inImgName,
                                 const char *_outImgName,
                                 int * _height,
                                 int * _width,
                                 int * _bitDepth,
                                 unsigned char * _header,
                                 unsigned char * _colorTable,
                                 unsigned char * _inBuf,
                                 unsigned char * _outBuf

                                 )
{
    inImgName  = _inImgName;
    outImgName = _outImgName;
    height     = _height;
    width      = _width;
    bitDepth   = _bitDepth;
    header     = _header;
    colorTable = _colorTable;
    inBuf      = _inBuf;
    outBuf     = _outBuf;
}

/**
 * @brief Read in an image from an input file.
 * @returns void
 */
void ImageProcessing::readImage()
{

    int i;
    FILE *streamIn;
    streamIn = fopen(inImgName,"rb");
    if(streamIn ==(FILE *)0)
    {
        cout<<"Unable to open file. Maybe file does not exist"<<endl;
        exit(0);
    }
    for(i=0;i<BMP_HEADER_SIZE;i++)
    {
        header[i] =  getc(streamIn);
    }
    *width = *(int *)&header[18];           //read the width from the image header
    *height = *(int *)&header[22];
    *bitDepth = *(int *)&header[28];

    if(*bitDepth <=8)
    {

        fread(colorTable,sizeof(unsigned char),BMP_COLOR_TABLE_SIZE,streamIn);
    }

    fread(inBuf,sizeof(unsigned char),_512by512_IMG_SIZE,streamIn );

    fclose(streamIn);
}

/**
 * @brief Write an image to a 512x512 .bmp file.
 * @returns void
 */
void ImageProcessing::writeImage(){

    FILE  *fo = fopen(outImgName,"wb");
    fwrite(header,sizeof(unsigned char),BMP_HEADER_SIZE,fo);
    if(*bitDepth <=8)
    {
        fwrite(colorTable,sizeof(unsigned char),BMP_COLOR_TABLE_SIZE,fo);
    }

   fwrite(outBuf, sizeof(unsigned char),_512by512_IMG_SIZE,fo);
   fclose(fo);
}

/**
 * @brief Iterating through the image and copying it to its destination.
 * @param _srcBuf source buffer array pointer of image pixels
 * @param _destBuf destination buffer array pointer of image pixels
 * @param bufSize integer representing size of source array
 * @return void
*/
void ImageProcessing ::copyImgData(unsigned char *_srcBuf, unsigned char *_destBuf, int bufSize)
{
    for(int i =0;i<bufSize;i++)
    {
        _destBuf[i] = _srcBuf[i];
    }
}

/**
 * @brief Iterating through the image and removing color from the image.
 * @param _inImgData - char pointer to input image data
 * @param _outImgData - char pointer to output image data
 * @param imgSize - integer representing size of source array
 * @param threshold - integer representing threshold between what's considered white or black
 * @return void
*/
void ImageProcessing::binarizeImage(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int threshold)
{
    for(int i=0;i<imgSize;i++)
    {
        _outImgData[i] =  (_inImgData[i] > threshold) ? WHITE :BLACK;
    }
}

void ImageProcessing::brightnessUp(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int brightness)
{

    for(int i =0;i<imgSize;i++)
    {
        int temp = _inputImgData[i]+ brightness;
        _outImgData[i] =  (temp > MAX_COLOR)? MAX_COLOR :temp;
    }
}

void ImageProcessing::brightnessDown(unsigned char *_inputImgData, unsigned char *_outImgData, int imgSize, int darkness)
{
     for(int i =0;i<imgSize;i++)
     {
         int temp = _inputImgData[i] - darkness;
         _outImgData[i] = (temp<MIN_COLOR) ? MIN_COLOR :temp;
     }
}

void ImageProcessing::computeHistogram(unsigned char * _imgData, int imgRows, int imgCols, float hist[])
{
    FILE *fptr;
    fptr =fopen("image_hist.txt","w");

    int x,y,i,j;
    long int ihist[255],sum;
    for(i =0;i<=255;i++)
    {
        ihist[i] =0;
    }
    sum =0;
    for(y=0;y<imgRows;y++)
    {
        for(x=0;x<imgCols;x++)
        {
            j = *(_imgData+x+y*imgCols);
            ihist[j] = ihist[j] +1;
            sum = sum+1;
        }

    }
    for(i=0;i<255;i++)
        hist[i] =  (float)ihist[i]/(float)sum;
  for(int i=0;i<255;i++)
    {
        fprintf(fptr,"\n%f",hist[i]);
    }
    fclose(fptr);
}

void ImageProcessing::equalizeHistogram(unsigned char * _inputImgData, unsigned char * _outputImgData, int imgRows, int imgCols)
{
    int x,y,i,j;
    int histeq[256];
    float hist[256];
    float sum ;

    const char initHist[] ="init_hist.txt";
    const char finalHist[] = "final_hist.txt";

    computeHistogram(&_inputImgData[0],imgRows,imgCols,&hist[0],initHist);
    for(i =0;i<=255;i++)
    {
        sum =0.0;
        for(j=0;j<=i;j++){
            sum = sum+hist[j];
        }
        histeq[i] =  (int)(255*sum+0.5);

    }
    for(y =0;y<imgRows;y++)
    {
        for(x=0;x<imgCols;x++)
        {
            *(_outputImgData+x+y*imgCols) = histeq[*(_inputImgData+x+y*imgCols)];
        }
    }
    computeHistogram(&_outputImgData[0], imgRows,imgCols,&hist[0],finalHist);
}

void ImageProcessing::getImageNegative(unsigned char *_inImgData, unsigned char * _outImgData,int imgWidth,int imgHeight)
{

    for(int i =0;i<imgHeight;i++)
    {
        for(int j=0;j<imgWidth;j++){
             _outImgData[i*imgWidth+j ]  = 255 - _inImgData[i*imgWidth+j];
        }
    }
}

ImageProcessing::~ImageProcessing()
{
    //dtor
}

