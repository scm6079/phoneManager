#ifndef LOADTARGA_H
#define LOADTARGA_H

#include <stdlib.h>
#include <stdio.h>
//#include <sys/io.h>
#include <stdint.h>
#include <memory.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

extern bool LoadTarga(const char *strFileName, WORD &wWidthOfImage, WORD &wHeightOfImage, WORD &wDepthInBytes, BYTE **pData);

#endif // LOADTARGA_H
