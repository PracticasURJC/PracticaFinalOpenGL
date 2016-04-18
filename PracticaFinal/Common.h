#include <cstdio>
#include <math.h>
#include <algorithm>
#include <ctime>
#include <vector>

#define _USE_MATH_DEFINES

#define MAX_HEIGHT      15
#define MAX_WIDTH       10
#define CENTER          4

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
