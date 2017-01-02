// platform_posix.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "racera.h"
#include "platform.h"

long
platform_executable_directory(char* destination, long destination_size)
{
    char proc_exe[32];
    snprintf(proc_exe, array_count(proc_exe), "/proc/%d/exe", getpid());

    char path_store[1024];
    int result = readlink(proc_exe, path_store, array_count(path_store));
    if(result == -1)
    {
        fprintf(stderr, "error %d when trying to read %s", result, proc_exe);
        invalid_code;
    }

    path_store[result] = 0;
    
    for(int i = result; i > 0; i--)
    {
        if(path_store[i] == '/')
        {
            path_store[i+1] = 0;
            break;
        }
    }

    long length_required = 0;
    long length_without_filename = strlen(path_store);
    if(length_without_filename < destination_size)
    {
        strcpy(destination, path_store);
    }
    else
    {
        length_required = length_without_filename + 1;
    }

    return length_required;
}

void
platform_set_working_directory(char* directory)
{
    int chdir_result = chdir(directory);
    if(chdir_result != 0)
    {
        fprintf(stderr, "couldn't set working directory to '%s'.", directory);
    }
}

void
platform_sleep(int milliseconds)
{
    usleep(milliseconds * 1000);
}
