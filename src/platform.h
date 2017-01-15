#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

typedef struct read_file read_file;

struct read_file
{
    unsigned long size;
    unsigned char* data;
};

long platform_executable_directory(char* destination, long destination_size);

void platform_set_working_directory(char* directory);

void platform_sleep(int milliseconds);

read_file platform_read_file(char* path, int append_null);

void platform_free_file(read_file* file);

void platform_random_seed(int seed);

int platform_random(int min, int max);

float platform_randomf(float min, float max);

void platform_copy_memory(void* destination, void* source, long size);

#endif // PLATFORM_H_INCLUDED
