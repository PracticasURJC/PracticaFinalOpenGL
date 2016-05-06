#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#define PlaySoundTetris PlaySound
#undef max
#else
#define PlaySoundCustom
#endif //_WIN32

#include <cstdio>
#include <math.h>
#include <algorithm>
#include <ctime>
#include <vector>
#include <stdlib.h>
#include <string>

#define _USE_MATH_DEFINES

#define MAX_HEIGHT                  15.0f
#define MAX_WIDTH                   10.0f
#define CENTER                      4.0f
#define NEXT_BLOCK_X                15.0f
#define NEXT_BLOCK_Y                8.0f
#define DISPLAY_NEXT_BLOCK_X        12.0f
#define DISPLAY_NEXT_BLOCK_Y        5.0f
#define DISPLAY_NEXT_BLOCK_HEIGHT   8.0f
#define DISPLAY_NEXT_BLOCK_WITDH    9.0f
#define LINE_PER_DIFF               5
#define NUM_BLOCK_SUBBLOCKS         4
#define POINTS_X                    -12.0f
#define POINTS_Y                    8.0f

#define BYTE_SIZE                   8
#define BUFFER_SIZE                 BYTE_SIZE * 4

#define WITH_DEBUG

#ifdef WITH_DEBUG
    #define DEBUG_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...) printf("", ##__VA_ARGS__)
#endif

typedef unsigned short uint8;
typedef signed short int8;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long long uint64;
typedef signed long long int64;
