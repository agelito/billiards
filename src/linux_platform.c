// linux_platform.c

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

    int i;
    for(i = result; i > 0; i--)
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

read_file
platform_read_file(char* path, int append_null)
{
    read_file result = (read_file){0};
    
    int file = open(path, O_RDONLY);
    if(file == -1)
    {
        printf("can't open file %s (%s)\n", path, strerror(errno));
        return result;
    }

    off_t file_size = lseek(file, 0, SEEK_END);
    if(file_size == -1)
    {
        printf("error retrieving file size %s (%s)\n", path, strerror(errno));
        close(file);
        return result;
    }

    lseek(file, 0, SEEK_SET);

    size_t data_size = file_size + (append_null ? 1 : 0);
    unsigned char* destination = (unsigned char*)malloc(data_size);

    ssize_t total_bytes_read = 0;
    while(total_bytes_read < file_size)
    {
        int MAX_READ_CHUNK = 4096;
        int bytes_left = (file_size - total_bytes_read);
	
        int read_size = MAX_READ_CHUNK;
        if(read_size > bytes_left)
        {
            read_size = bytes_left;
        }

	unsigned char* read_to = (destination + total_bytes_read);
        ssize_t bytes_read = read(file, read_to, read_size);
        if(bytes_read == -1)
        {
            printf("error reading file %s (%s)\n", path, strerror(errno));
	    free(destination);
            close(file);

            return result;
        }

        total_bytes_read += bytes_read;
    }

    close(file);

    if(append_null)
    {
	*(destination + file_size) = 0;
    }

    result.size = file_size;
    result.data = destination;

    return result;
}

void
platform_free_file(read_file* file)
{
    if(file->data)
    {
	free(file->data);
    }

    *file = (read_file){0};
}

void
platform_random_seed(int seed)
{
    srandom((unsigned int)seed);
}

int
platform_random(int min, int max)
{
    long int random_number = random();
    
    int delta = (max - min);
    int range = delta * ((float)random_number / RAND_MAX);
    
    return (min + range);
}

float
platform_randomf(float min, float max)
{
    long int random_number = random();

    float delta = (max - min);
    float range = delta * ((float)random_number / RAND_MAX);
    
    return (min + range);
}

void
platform_copy_memory(void* destination, void* source, long size)
{
    memcpy(destination, source, size);
}


