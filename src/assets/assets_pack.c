// assets_pack.c
// Combine multiple files into one archive.

// TODO:
// - CRC data for integrity checking.
// - Varying endianess support.
// - Configure max archive size.
// - File paths relative to directory provided as argument.
// - Compress file archive.
// - Configure file data alignment.
// - Configure log verbosity.

#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ftw.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define for_range(n, count) for(n = 0; n < count; ++n)
#define unused_arg(arg) (void)arg;

#define KB(kilo_bytes) kilo_bytes * 1024
#define MB(mega_bytes) KB(mega_bytes) * 1024
#define GB(giga_bytes) MB(giga_bytes) * 1024
#define TB(tera_bytes) (u64)GB(tera_bytes) * 1024

#define TO_KB(bytes) bytes / 1024.0
#define TO_MB(bytes) TO_KB(bytes) / 1024.0
#define TO_GB(bytes) TO_MB(bytes) / 1024.0
#define TO_TB(bytes) TO_GB(bytes) / 1024.0

#include "../math.c"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;

typedef struct ap_file
{
    u32 path_hash;
    char* path;
    u64 size;
} ap_file;

typedef struct ap_filelist
{
    int count;
    int capacity;
    ap_file* files;
} ap_filelist;

typedef struct ap_configuration
{
    char* output_file;

    int path_count;
    char** paths;

    ap_filelist filelist;
} ap_configuration;

// TODO: No thread safety modifying this global variable. It's used
// when traversing the directory tree using ntfw callback function.
static ap_filelist* global_filelist = 0;

int
filelist_contains(ap_filelist* filelist, u32 path_hash)
{
    int file_index, count = 0;

    for(file_index = 0; file_index < filelist->count; ++file_index)
    {
	ap_file* file = (filelist->files + file_index);
	if(file->path_hash == path_hash)
	{
	    ++count;
	}
    }

    return count;
}

void
filelist_expand(ap_filelist* filelist, int new_capacity)
{
    int copy_count = filelist->count;
    ap_file* original_files = 0;
    
    if(copy_count > 0)
    {
	original_files = (ap_file*)malloc(sizeof(ap_file) * copy_count);
	if(original_files != 0)
	{
	    memcpy(original_files, filelist->files, sizeof(ap_file) * copy_count);
	}
	else
	{
	    printf("error: couldn't preserve original files when appending.\n");
	}
    }

    if(filelist->files)
    {
	free(filelist->files);
    }

    filelist->files = (ap_file*)malloc(sizeof(ap_file) * new_capacity);
    
    if(!filelist->files)
    {
	printf("error: couldn't allocate memory for file list.\n");
	if(original_files) free(original_files);
	filelist->capacity = 0;
	filelist->count = 0;
	return;
    }
    
    filelist->capacity = new_capacity;

    if(original_files)
    {
	memcpy(filelist->files, original_files, sizeof(ap_file) * copy_count);
	free(original_files);
    }
}

void
filelist_append(ap_filelist* filelist, ap_file file)
{
    if(filelist->count == filelist->capacity)
    {
	filelist_expand(filelist, filelist->capacity + 1024);
	if(filelist->count == filelist->capacity)
	{
	    printf("couldn't append file: %s\n", file.path);
	    return;
	}
    }

    *(filelist->files + filelist->count++) = file;
}

void
filelist_free(ap_filelist* filelist)
{
    if(filelist->files)
    {
	int n;
	for(n = 0; n < filelist->count; ++n)
	{
	    ap_file* file = (filelist->files + n);
	    if(file->path) free(file->path);
	    
	}

	free(filelist->files);
    }

    filelist->capacity = 0;
    filelist->count = 0;
}

char*
read_symlink(char* path)
{
    char* target;
    
    size_t max_length = 1024;
    ssize_t length;

    while(1)
    {
	target = malloc(max_length + 1);
	if(target == 0) break;

	length = readlink(path, target, max_length);
	if(length == (ssize_t)-1)
	{
	    free(target);
	    target = 0;
	    break;
	}

	if(length >= (ssize_t)max_length)
	{
	    free(target);
	    max_length += 1024;
	    continue;
	}

	target[length] = '\0';
	break;
    }

    return target;
}

char*
copy_string(char* input)
{
    // TODO: Is this safe for all kind of string encodings?
    
    size_t length = strlen(input) + 1;
    char* output = (char*)malloc(sizeof(char) * length);
    
    strncpy(output, input, length);

    return output;
}

void
print_file_size(u64 file_size)
{
    if(file_size > TB(1))
    {
	printf("%9.7f TB", (double)TO_TB(file_size));
    }
    else if(file_size > GB(1))
    {
	printf("%9.6f GB", (double)TO_GB(file_size));
    }
    else if(file_size > MB(1))
    {
	printf("%9.5f MB", (double)TO_MB(file_size));
    }
    else if(file_size > KB(1))
    {
	printf("%9.3f KB", (double)TO_KB(file_size));
    }
    else
    {
	printf("%9lu B", file_size);
    }
}

int
traverse_file_callback(const char* path, const struct stat* info,
		       const int type, struct FTW* path_info)
{
    unused_arg(path_info);

    if(type == FTW_F)
    {
	u32 path_hash = hash_string((char*)path);
	if(!filelist_contains(global_filelist, path_hash))
	{
	    ap_file file_data = (ap_file){0};
	    file_data.path = copy_string((char*)path);
	    file_data.path_hash = path_hash;
	    file_data.size = (u64)info->st_size;
	    
	    filelist_append(global_filelist, file_data);
	}
    }
    else if(type == FTW_D || type == FTW_DP)
    {

    }
    else if(type == FTW_SL)
    {
	char* symlink_target = read_symlink((char*)path);
	printf("%s -> %s : ", path, symlink_target);
	printf("symlinks is not supported, %s will be ignored.\n", symlink_target);
	if(symlink_target)
	    free(symlink_target);
    }
    else if(type == FTW_SLN)
    {
	printf("invalid symlink: %s\n", path);
    }
    else
    {
	if(type == FTW_DNR)
	{
	    printf("can't read directory: %s\n", path);
	}
	else
	{
	    printf("can't read file: %s", path);
	}
    }

    return 0;
}

#define AP_FILE_DESCRIPTOR_LIMIT 32

void
traverse_directory(char* path)
{
    if(path == 0 || *path == '\0')
    {
	printf("can't traverse directory, path is null or empty.\n");
	return;
    }

    int result = nftw(path, traverse_file_callback, AP_FILE_DESCRIPTOR_LIMIT, FTW_PHYS);
    if(result != 0)
    {
	printf("failed traversing directory \n error: %d\n path:  %s\n", result, path);
    }
}

#undef AP_FILE_DESCRIPTOR_LIMIT

void
print_usage(char* exe_name)
{
    const char* usage_message_format = "usage: ./%s [input_path(s)] <output_file>\n";

    printf(usage_message_format, exe_name);
}

void
parse_arguments(int argument_count, char* arguments[], ap_configuration* configuration)
{
    int path_count = 0;
    if(argument_count > 2)
    {
	path_count = (argument_count - 2);
    }

    configuration->path_count = path_count;
    configuration->paths = (char**)malloc(sizeof(char*) * path_count);

    int last = argument_count-1;
    configuration->output_file = arguments[last];

    int path_start = 1;
    
    int path_index;
    for(path_index = 0; path_index < path_count; ++path_index)
    {
	char* path = arguments[path_start + path_index];
	configuration->paths[path_index] = path;
    }
}

void free_configuration(ap_configuration* configuration)
{
    if(configuration->paths)
    {
	free(configuration->paths);
	configuration->paths = 0;
    }

    filelist_free(&configuration->filelist);
}

int
main(int argc, char* argv[])
{
    char* executable = 0;
    if(argc > 0)
    {
	executable = argv[0];
    }
    
    ap_configuration configuration = (ap_configuration){0};

    parse_arguments(argc, argv, &configuration);

    if(!configuration.output_file)
    {
	printf("no output file.\n");
	print_usage(executable);
    }

    global_filelist = &configuration.filelist;

    int path_index;
    for(path_index = 0; path_index < configuration.path_count; ++path_index)
    {
	traverse_directory(configuration.paths[path_index]);
    }

    global_filelist = 0;

    u32 file_count = configuration.filelist.count;
    u64* file_data_offsets = (u64*)malloc(sizeof(u64) * file_count);
    
    if(!file_data_offsets)
    {
	printf("couldn't allocate memory for data offsets.");
	free_configuration(&configuration);
	return -1;
    }

    // NOTE:                 Hash          Offset        Size          Path Length
    u32 lookup_entry_size = (sizeof(u32) + sizeof(u64) + sizeof(u64) + sizeof(u16));

    u64 lookup_size = (lookup_entry_size * file_count);

    u64 file_data_size = 0;
					  
    u32 file_index;
    for(file_index = 0; file_index < file_count; ++file_index)
    {
	ap_file* file = (configuration.filelist.files + file_index);
	printf("%08x %-40s    ", file->path_hash, file->path);
	print_file_size(file->size);
	printf("\n");

	lookup_size += sizeof(char) * strlen(file->path);

	*(file_data_offsets + file_index) = file_data_size;

	file_data_size += file->size;
    }

    printf("archive information\n");
    printf("%-18s", " total size: ");
    print_file_size(lookup_size + file_data_size);
    printf("\n");
    printf("%-18s", " lookup size: ");
    print_file_size(lookup_size);
    printf("\n");
    printf("%-18s", " file data size: ");
    print_file_size(file_data_size);
    printf("\n");

    FILE* archive_file = fopen(configuration.output_file, "wb");
    if(archive_file == 0)
    {
	printf("couldn't open archive file for writing: %s\n", configuration.output_file);
	free(file_data_offsets);
	free_configuration(&configuration);
	return -1;
    }

    for(file_index = 0; file_index < file_count; ++file_index)
    {
	ap_file* file = (configuration.filelist.files + file_index);
	u64 offset = *(file_data_offsets + file_index) + lookup_size;

	// TODO: Is this safe for all kind of string encodings?
	
	u16 path_length = strlen(file->path);
	u16 path_size = sizeof(char) * path_length;

	fwrite(&file->path_hash, 1, sizeof(u32), archive_file);
	fwrite(&offset, 1, sizeof(u64), archive_file);
	fwrite(&file->size, 1, sizeof(u64), archive_file);
	fwrite(&path_size, 1, sizeof(u16), archive_file);
	fwrite(file->path, 1, path_size, archive_file);
    }

    char read_buffer[4096];

    for(file_index = 0; file_index < file_count; ++file_index)
    {
	ap_file* file = (configuration.filelist.files + file_index);

	FILE* source_file = fopen(file->path, "rb");

	size_t bytes_left = file->size;
	while(bytes_left > 0)
	{
	    size_t bytes_read = fread(read_buffer, 1, 4096, source_file);
	    if(bytes_read == 0)
	    {
		printf("error reading file: %s\n", file->path);
		break;
	    }

	    size_t bytes_written = fwrite(read_buffer, 1, bytes_read, archive_file);
	    if(bytes_written != bytes_read)
	    {
		printf("error writing to archive: %s\n", configuration.output_file);
		break;
	    }

	    bytes_left -= bytes_written;
	}

	fclose(source_file);
    }

    fclose(archive_file);

    free(file_data_offsets);
    
    free_configuration(&configuration);

    return 0;
}
