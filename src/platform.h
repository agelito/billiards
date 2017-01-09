#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

long platform_executable_directory(char* destination, long destination_size);

void platform_set_working_directory(char* directory);

void platform_sleep(int milliseconds);

int platform_read_file(char* path, char* destination, int destination_size);

void platform_random_seed(int seed);

int platform_random(int min, int max);

float platform_randomf(float min, float max);

void platform_copy_memory(void* destination, void* source, long size);

#endif // PLATFORM_H_INCLUDED
