#include "files.h"
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool parsePNG(u8** data, u32* width, u32* height, const char* filename)
{
  unsigned error;

  error = lodepng_decode32_file(data, width, height, filename);
  if (error != 0)
  {
    printf("Failed to decode png '%s'\n", filename);
    return false;
  }

  return true;
}

bool readFile(char** buffer, int* len, const char* fileName)
{
  FILE* filePtr;
  long  fileSize, count;
  int   error;

  filePtr = fopen(fileName, "r");
  if (!filePtr)
  {
    printf("Failed to open file at '%s'\n", fileName);
    return false;
  }

  fileSize            = fseek(filePtr, 0, SEEK_END);
  fileSize            = ftell(filePtr);

  *len                = fileSize;
  *buffer             = (char*)malloc(sizeof(char) * (fileSize + 1));
  (*buffer)[fileSize] = '\0';
  fseek(filePtr, 0, SEEK_SET);
  count = fread(*buffer, 1, fileSize, filePtr);
  if (count != fileSize)
  {
    free(*buffer);
    printf("count != fileSize %ld %ld\n", count, fileSize);
    return false;
  }

  error = fclose(filePtr);
  if (error != 0)
  {
    free(*buffer);
    return false;
  }

  return true;
}

bool parseTarga(u8** data, u32* width, u32* height, const char* filename)
{

  struct TargaHeader targaFileHeader;

  FILE*              filePtr;
  unsigned long      count, imageSize;
  unsigned char*     targaData;
  unsigned char*     targaImage;

  // Open the targa file for reading in binary.
  filePtr = fopen(filename, "rb");
  if (filePtr == NULL)
  {
    printf("ERROR: file doesn't exist %s\n", filename);
    return false;
  }

  // Read in the file header.
  count = fread(&targaFileHeader, sizeof(struct TargaHeader), 1, filePtr);
  if (count != 1)
  {
    printf("ERROR: Failed to read into header\n");
    return false;
  }

  // Get the important information from the header.
  *width  = (u32)targaFileHeader.width;
  *height = (u32)targaFileHeader.height;

  // Calculate the size of the 32 bit image data.
  if (targaFileHeader.imagePixelSize == 32)
  {
    imageSize = targaFileHeader.width * targaFileHeader.height * 4;
    printf("32\n");
  }
  else if (targaFileHeader.imagePixelSize == 24)
  {
    imageSize = targaFileHeader.width * targaFileHeader.height * 3;
    printf("24\n");
  }
  else
  {
    printf("Dont't know how to parse targa image with bpp of '%d'\n", targaFileHeader.imagePixelSize);
    return false;
  }

  // Allocate memory for the targa image data.
  targaImage = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

  // Read in the targa image data.
  count = fread(targaImage, 1, imageSize, filePtr);
  if (count != imageSize)
  {
    printf("ERROR: count read doesn't equal imageSize\n");
    return false;
  }

  if (fclose(filePtr) != 0)
  {
    return false;
  }

  targaData   = (u8*)malloc(sizeof(u8) * imageSize);
  bool bit32  = targaFileHeader.imagePixelSize == 32;

  u32  maxIdx = targaFileHeader.height * targaFileHeader.width;
  for (u32 idx = 0; idx < maxIdx; idx++)
  {
    targaData[idx]     = targaImage[idx + 2]; // Red
    targaData[idx + 1] = targaImage[idx + 1]; // Green
    targaData[idx + 2] = targaImage[idx];     // Blue
    if (bit32)
    {
      targaData[idx + 3] = targaImage[idx + 3]; // Alpha
    }
  }

  free(targaImage);
  *data = targaData;
    return true;
}
