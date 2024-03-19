
#include "../src/common/common.h"
#include "../src/common/files.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TILE_FOLDER_LOCATION "../Assets/Tiles/Platformer/"

int compareStrings(const void* a, const void* b)
{
  return strcmp(*(const char**)a, *(const char**)b);
}

void saveTarga(struct Image* image, const char* filename)
{
  printf("Saving at '%s'\n", filename);
  FILE*              filePtr;
  unsigned long      count;

  struct TargaHeader h;
  memset(&h.header[0], 0, 18);
  h.width          = image->width;
  h.height         = image->height;
  h.imageType      = 2;
  h.imagePixelSize = 32;

  filePtr          = fopen(filename, "wb");
  if (!filePtr)
  {
    printf("Failed to open file\n");
    exit(2);
  }
  count = fwrite(&h, sizeof(struct TargaHeader), 1, filePtr);
  if (count != 1)
  {
    printf("ERROR: Failed to write into header\n");
    return;
  }

  u32 imageSize = 4 * image->width * image->height;

  // Read in the targa image data.
  count = fwrite(image->data, 1, imageSize, filePtr);
  if (count != imageSize)
  {
    printf("ERROR: count after write doesn't equal imageSize %ld %d\n", count, imageSize);
    return;
  }

  if (fclose(filePtr) != 0)
  {
    printf("Failed to close file\n");
    return;
  }
}

int main()
{
  DIR*           directory;
  struct dirent* entry;

  directory = opendir(TILE_FOLDER_LOCATION);

  if (directory == NULL)
  {
    printf("Couldn't find dir '%s'\n", TILE_FOLDER_LOCATION);
    exit(2);
  }

  char  tileFilename[256];
  char* tileFilenames[256];
  int   count           = 0;

  int   folderStringLen = strlen(TILE_FOLDER_LOCATION);
  while ((entry = readdir(directory)) != NULL)
  {
    memset(tileFilename, 0, 256);
    strcpy(tileFilename, TILE_FOLDER_LOCATION);
    strcpy(&tileFilename[folderStringLen], entry->d_name);

    if (memcmp(&tileFilename[strlen(tileFilename) - 4], ".png", 5) == 0)
    {
      tileFilenames[count] = (char*)malloc(sizeof(char) * (strlen(tileFilename) + 1));
      strcpy(tileFilenames[count], tileFilename);
      tileFilenames[count][strlen(tileFilename)] = '\0';
      count++;
    }
  }
  closedir(directory);

  qsort(&tileFilenames[0], count, sizeof(char*), compareStrings);

  struct Image sheet;
  u8           dim = 18;
  sheet.bpp        = 4;
  sheet.width      = 10 * dim;
  sheet.height     = (count / 10 + 1) * dim;

  sheet.data       = (u8*)malloc(sizeof(u8) * count * dim * sheet.bpp * dim * 2);

  struct Image tile;
  u32          tileDimBytes = dim * sheet.bpp;
  i32          layer;
  printf("Count was %d\n", count);
  for (i32 i = 0; i < count; i++)
  {
    bool result = parsePNG(&tile.data, &tile.width, &tile.height, tileFilenames[i]);
    if (!result)
    {
      continue;
    }
    layer = i / 10;
    for (u8 y = 0; y < dim; y++)
    {
      i32 tileRowOffset  = y * sheet.width * sheet.bpp;
      i32 imageColOffset = (i % 10) * tileDimBytes;
      i32 imageRowOffset = layer * sheet.width * sheet.bpp * dim;
      u8* curr           = &sheet.data[tileRowOffset + imageColOffset + imageRowOffset];
      memcpy(&curr[0], &tile.data[y * tileDimBytes], tileDimBytes);
      for (i32 j = 0; j < tileDimBytes; j += 4)
      {
        u8 j0       = curr[j + 0];
        u8 j1       = curr[j + 1];
        u8 j2       = curr[j + 2];
        u8 j3       = curr[j + 3];
        curr[j + 0] = j2;
        curr[j + 1] = j1;
        curr[j + 2] = j0;
        curr[j + 3] = j3;
      }
    }
  }
  printf("Layers: %d\n", count / 10);

  saveTarga(&sheet, "tilesheet.tga");
}
