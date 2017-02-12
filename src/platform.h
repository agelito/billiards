#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint32 bool32;

typedef float real32;
typedef double real64;

typedef struct read_file read_file;

struct read_file
{
    unsigned long size;
    unsigned char* data;
};

long
platform_executable_directory(char* destination, long destination_size);

void
platform_set_working_directory(char* directory);

void
platform_sleep(int milliseconds);

read_file
platform_read_file(char* path, int append_null);

void
platform_free_file(read_file* file);

void
platform_random_seed(int seed);

int
platform_random(int min, int max);

float
platform_randomf(float min, float max);

void
platform_copy_memory(void* destination, void* source, long size);

void
platform_log(char* format, ...);

long
platform_format(char* destination, long size, char* format, ...);

#endif // PLATFORM_H_INCLUDED
